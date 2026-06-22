*This project has been created as part of the 42 curriculum by romgutie.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic dining philosophers problem, reframed in a modern collaborative coding environment. Multiple coders sit around a shared Quantum Compiler, and each must acquire two USB dongles simultaneously to compile their code. The simulation exercises POSIX thread synchronization: coders must compile regularly or they burn out, and the program must guarantee fairness, liveness, and precise burnout detection.

The simulation stops either when a coder burns out or when every coder has compiled at least a required number of times.

Key properties of the simulation:
- Each coder is represented by a POSIX thread (`pthread_create`)
- Each dongle is protected by a mutex and a condition variable
- Dongles have a configurable cooldown period after release
- Two arbitration policies are supported: **FIFO** and **EDF** (Earliest Deadline First)
- A dedicated monitor thread detects burnout and halts the simulation within 10 ms

## Instructions

### Compilation

```bash
make
```

This produces the `codexion` binary at the project root.

To clean object files:
```bash
make clean
```

To remove all build artifacts:
```bash
make fclean
```

To recompile from scratch:
```bash
make re
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

**Arguments** (all mandatory, all positive integers except `scheduler`):

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) in the simulation |
| `time_to_burnout` | Milliseconds before a coder burns out without compiling |
| `time_to_compile` | Milliseconds spent compiling (dongle held the whole time) |
| `time_to_debug` | Milliseconds spent debugging after each compile |
| `time_to_refactor` | Milliseconds spent refactoring before attempting to compile again |
| `number_of_compiles_required` | Target number of compiles per coder before clean exit |
| `dongle_cooldown` | Milliseconds a dongle is unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### Examples

```bash
# 4 coders, 800ms burnout, 200ms compile, 200ms debug, 200ms refactor, 5 compiles, 0ms cooldown, FIFO
./codexion 4 800 200 200 200 5 0 fifo

# 3 coders, 600ms burnout, 150ms compile, 150ms debug, 150ms refactor, 3 compiles, 50ms cooldown, EDF
./codexion 3 600 150 150 150 3 50 edf

# Edge case: single coder (burns out immediately — only one dongle, can never compile)
./codexion 1 500 100 100 100 5 0 fifo
```

### Expected log format

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
402 1 is refactoring
405 2 has taken a dongle
406 2 has taken a dongle
406 2 is compiling
```

## Blocking Cases Handled

### Deadlock prevention

The classic deadlock scenario in the dining philosophers problem occurs when every coder simultaneously picks up their left dongle and waits indefinitely for their right one, causing a circular wait — one of Coffman's four necessary conditions for deadlock.

This implementation breaks the circular wait by having even-numbered coders pick up their **left** dongle first and odd-numbered coders pick up their **right** dongle first. This asymmetric acquisition order ensures that at least one coder in any adjacent pair will always be able to proceed, eliminating the possibility of a circular hold-and-wait.

The four Coffman conditions and how they are addressed:
- **Mutual exclusion**: Required by design (dongles are exclusive). Not eliminated.
- **Hold and wait**: Partially mitigated by the odd/even pickup strategy.
- **No preemption**: Dongles are never forcibly taken. Not eliminated, but deadlock is avoided structurally.
- **Circular wait**: **Eliminated** by the asymmetric acquisition order.

### Starvation prevention

Under **FIFO** scheduling, requests are served strictly in arrival order, guaranteeing that every waiting coder eventually gets access to the dongle regardless of how many competitors exist.

Under **EDF** scheduling, the coder with the nearest burnout deadline is always served first. This policy prioritizes the most urgent coder and is designed to prevent burnout-driven starvation. A liveness guarantee is maintained provided the simulation parameters are feasible (i.e., `time_to_compile + time_to_debug + time_to_refactor < time_to_burnout` with enough slack for scheduling).

Both policies are implemented via a custom binary min-heap priority queue (no standard library priority queue used), stored per dongle.

### Cooldown handling

After a coder releases a dongle, it is marked unavailable until `dongle_cooldown` milliseconds have elapsed (`available_at = get_time() + dongle_cooldown`). The condition variable loop in `take_dongle` checks `get_time() >= dongle->available_at` before granting access, so a coder at the head of the queue will remain blocked until the cooldown expires, then be woken by the next `pthread_cond_broadcast`.

### Precise burnout detection

A dedicated monitor thread polls every millisecond and compares the current time against each coder's `last_compile_ms + time_to_burnout`. Because the monitor runs independently and sleeps for only 1 ms between checks, burnout is reliably detected and logged within the required 10 ms tolerance. Once burnout is detected, `simulation_over` is set and all dongle condition variables are broadcast to unblock any waiting coder threads immediately.

### Log serialization

All output goes through `log_state`, which acquires `sim->log_mutex` before calling `printf` and releases it afterward. This guarantees that no two log lines can interleave on stdout, even when multiple coder threads and the monitor thread all attempt to log simultaneously.

## Thread Synchronization Mechanisms

### `pthread_mutex_t` — exclusive access to shared state

Three categories of mutexes are used:

- **`dongle->mutex`** (one per dongle): Guards the dongle's `available_at` field and its priority queue. Any thread that reads or modifies dongle state must hold this mutex. Prevents race conditions between competing coder threads trying to enqueue requests or check availability.
- **`coder->compile_mutex`** (one per coder): Protects `last_compile_ms` and `compile_count`. The monitor thread reads these fields to check burnout and completion; the coder thread writes them after each compile. Without this mutex, the monitor could observe a partially updated `last_compile_ms` and incorrectly declare burnout.
- **`sim->log_mutex`**: Serializes all writes to stdout. Without it, partial `printf` output from two threads could be interleaved on a single terminal line.
- **`sim->state_mutex`**: Protects `simulation_over`. Both the monitor and coder threads read this flag; the monitor writes it. The mutex ensures the flag is always read and written atomically.

### `pthread_cond_t` — blocking wait with mutex integration

Each dongle has a condition variable (`dongle->cond`) that coders use to sleep while waiting their turn. The pattern in `take_dongle` is:

```c
pthread_mutex_lock(&dongle->mutex);
// enqueue request
while (/* not our turn or cooldown not expired */)
    pthread_cond_wait(&dongle->cond, &dongle->mutex);
// take the dongle
pthread_mutex_unlock(&dongle->mutex);
```

`pthread_cond_wait` atomically releases the mutex and suspends the thread, preventing a busy-wait loop. When `release_dongle` or the monitor calls `pthread_cond_broadcast`, all waiting coders on that dongle are woken and re-evaluate the condition. Only the coder at the head of the priority queue with a cleared cooldown will proceed; all others go back to sleep.

### Race condition prevention — examples

**Example 1: compile_count update**
```c
// coder thread (coders.c)
pthread_mutex_lock(&coder->compile_mutex);
coder->last_compile_ms = get_time();
coder->compile_count++;
pthread_mutex_unlock(&coder->compile_mutex);

// monitor thread (monitor.c)
pthread_mutex_lock(&sim->coders[i].compile_mutex);
if (get_time() - sim->coders[i].last_compile_ms > sim->time_to_burnout)
    // burnout
pthread_mutex_unlock(&sim->coders[i].compile_mutex);
```
Without the mutex, the monitor could read `last_compile_ms` mid-update, getting an inconsistent value and falsely triggering burnout.

**Example 2: simulation_over flag**
```c
// monitor sets the flag
pthread_mutex_lock(&sim->state_mutex);
sim->simulation_over = 1;
pthread_mutex_unlock(&sim->state_mutex);

// any thread reads the flag via is_sim_over()
pthread_mutex_lock(&sim->state_mutex);
res = sim->simulation_over;
pthread_mutex_unlock(&sim->state_mutex);
```
Every read and write of `simulation_over` goes through `state_mutex`, making the flag effectively atomic across all threads.

### Custom priority queue (binary min-heap)

Each dongle owns a `t_pqueue` (binary min-heap). When a coder calls `take_dongle`, it pushes a `t_request` containing its `coder_id`, `arrival_ms`, and `deadline_ms` onto the heap. The heap orders requests by arrival time (FIFO) or deadline (EDF). The coder only exits the wait loop when it is at the top of the heap (`pqueue_peek().coder_id == coder->id`), ensuring strict policy compliance. Since all heap operations happen under `dongle->mutex`, no concurrent modification is possible.

## Resources

### Documentation and references

- youtube video
- `man pthread_create`, `man pthread_mutex_init`, `man pthread_cond_wait`, `man gettimeofday`

### AI usage

AI was used during this project for the following tasks:

- **Conceptual clarification**: Asking for explanations of EDF scheduling and tie-breaking strategies, and discussing trade-offs between FIFO and EDF in terms of starvation risk.
- **Debugging assistance**: Describing observed race conditions (e.g., false burnout detection) and asking for analysis of potential causes, then manually reviewing and applying the insights.
- **README drafting**: Generating the initial structure and content of this README based on the project subject and source code, then reviewing and adjusting for accuracy.

All AI-generated content was reviewed, understood, and validated before being included in the project. No code was copied from AI output without being read, understood, and tested independently.
