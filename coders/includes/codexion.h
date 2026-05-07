/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 13:39:15 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/07 16:04:45 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef long long		t_ms;
typedef struct s_sim	t_sim;
typedef struct s_dongle	t_dongle;

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_request
{
	int		coder_id;
	t_ms	arrival_ms;
	t_ms	deadline_ms;
}	t_request;

typedef struct s_pqueue
{
	t_request	*data;
	int			size;
	int			capacity;
	t_scheduler	scheduler;
}	t_pqueue;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_ms			available_at;
	t_pqueue		*queue;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compile_count;
	pthread_mutex_t	compile_mutex;
	t_ms			last_compile_ms;
	int				burned_out;
	t_dongle		*left;
	t_dongle		*right;
	t_sim			*sim;
	pthread_t		thread;
}	t_coder;

typedef struct s_sim
{
	int				nb_coders;
	t_ms			time_to_burnout;
	t_ms			time_to_compile;
	t_ms			time_to_debug;
	t_ms			time_to_refactor;
	int				nb_compiles_required;
	t_ms			dongle_cooldown;
	t_scheduler		scheduler;
	int				simulation_over;
	t_ms			start_ms;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		*threads;
	pthread_t		monitor;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	state_mutex;
}	t_sim;

long long	get_time(void);
int			is_sim_over(t_sim *sim);
int			parse_sim_args(t_sim *sim, int ac, char **av);
t_sim		*init_sim(int ac, char **av);
int			init_dongles(t_sim *sim);
void		init_coders(t_sim *sim);
t_pqueue	*init_pqueue(t_scheduler scheduler);
void		destroy_pqueue(t_pqueue *queue);
int			resize_pqueue(t_pqueue *queue);
int			has_priority(t_request req_a, t_request req_b, t_scheduler sched);
void		sift_up(t_pqueue *queue);
int			pqueue_push(t_pqueue *queue, t_request req);
void		sift_down(t_pqueue *queue);

#endif
