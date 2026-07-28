*This project has been created as part of the 42 curriculum by romgutie.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the dining philosophers problem. Coders sit around a shared compiler, each needing two USB dongles simultaneously to compile. Coders must compile regularly or burn out. The simulation stops when a coder burns out or when every coder has compiled a required number of times.

- Each coder is a POSIX thread; each dongle has a mutex + condition variable
- Configurable dongle cooldown after release
- FIFO or EDF (Earliest Deadline First) arbitration
- A monitor thread detects burnout and halts the simulation

## Build & Run

```bash
make        # builds ./codexion
make clean / fclean / re
```

```bash
./codexion nb_coders time_to_burnout time_to_compile time_to_debug time_to_refactor nb_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `nb_coders` | Number of coders (and dongles) |
| `time_to_burnout` | ms before burnout without compiling |
| `time_to_compile` | ms spent compiling (dongle held) |
| `time_to_debug` / `time_to_refactor` | ms spent after compiling |
| `nb_compiles_required` | Target compiles per coder |
| `dongle_cooldown` | ms a dongle stays unavailable after release |
| `scheduler` | `fifo` or `edf` |

```bash
./codexion 4 800 200 200 200 5 0 fifo
./codexion 1 500 100 100 100 5 0 fifo   # single coder: can never compile, always burns out
```

**Feasibility note:** worst-case wait for a contested dongle ≈ `time_to_compile + time_to_debug + time_to_refactor + dongle_cooldown`. With an odd number of coders, a coder can hit this worst case twice in a row before compiling once. Keep `time_to_burnout` well above ~2× that value, or a burnout may be legitimate contention rather than a bug.

## Deadlock & Starvation

**Deadlock:** even-ID coders take `left` then `right`; odd-ID coders take `right` then `left`. This breaks the circular wait (Coffman's 4th condition) — even in the 2-coder case, both end up contending for the *same* dongle first instead of each holding one and waiting on the other.

**Starvation:** FIFO serves strictly by arrival order. EDF serves by nearest deadline, arrival time as tie-breaker. Both are implemented with a custom binary min-heap, one per dongle (never resizes in practice — each dongle has at most 2 contenders in this topology).

**Cooldown:** `available_at = get_time() + dongle_cooldown`, checked before granting access.

**Log serialization:** `log_state` holds `log_mutex` around the whole check+print, so no two lines interleave.

## Synchronization

- **`dongle->mutex`**: guards `available_at`, `held`, and the queue.
- **`held`**: explicit flag, set on acquire / cleared on release. The queue alone decides *who's next*; `held` guarantees only *one* coder actually holds the dongle at a time — without it, a new request could slip into a momentarily empty queue before the previous holder released.
- **`compile_mutex`** (per coder): protects `last_compile_ms`/`compile_count`, read by the monitor, written by the coder.
- **`state_mutex`**: protects `simulation_over`.

**Condition wait:**
```c
while (!held && at_head_of_queue && cooldown_expired)
    pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &deadline); // 5ms
```
A plain `pthread_cond_wait` only re-checks on broadcast. Since the exit condition also depends on elapsed time (`available_at`), a coder could miss its window if the one broadcast it received arrived before the cooldown expired, and never get woken again if no further release follows. `timedwait` with a short bounded timeout turns this into a bounded poll, so a coder can never sleep forever waiting on time alone.

## Notes

A few unused leftovers from earlier design iterations were removed after verifying they were truly unreachable: `t_coder.burned_out`, `t_dongle.id`, `sim->threads` (thread handles live in `t_coder.thread`), `destroy_pqueue` (duplicated by `cleanup`), and `resize_pqueue` (queue never exceeds 2 entries given the topology — confirmed safe with AddressSanitizer under load).

## Resources

- `man pthread_create`, `pthread_mutex_init`, `pthread_cond_timedwait`, `gettimeofday`

### AI usage

AI was used for conceptual clarification (EDF/FIFO tie-breaking, starvation trade-offs), debugging assistance (walking through the code and simulation output to identify and fix a use-after-free in `main.c`, a missing mutual-exclusion guarantee on dongles, and a missed-wakeup bug in the condition wait), code review (auditing for unused fields/functions), and README drafting. All suggestions were reviewed, understood, and independently tested before inclusion.
