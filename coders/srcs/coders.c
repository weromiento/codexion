/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 11:31:07 by romgutie          #+#    #+#             */
/*   Updated: 2026/08/18 11:45:29 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request		req;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->mutex);
	req.coder_id = coder->id;
	req.arrival_ms = get_time();
	req.deadline_ms = coder->last_compile_ms + coder->sim->time_to_burnout;
	pqueue_push(dongle->queue, req);
	while (1)
	{
		if (is_sim_over(coder->sim))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		if (!dongle->held && dongle->queue->size > 0
			&& pqueue_peek(dongle->queue).coder_id == coder->id
			&& get_time() >= dongle->available_at)
			break ;
		build_deadline(&ts, 5);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	pqueue_pop(dongle->queue);
	dongle->held = 1;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->held = 0;
	dongle->available_at = get_time() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

static void	take_dongles(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		take_dongle(coder, coder->left);
		log_state(coder->sim, coder->id, "has taken a dongle");
		take_dongle(coder, coder->right);
	}
	else
	{
		take_dongle(coder, coder->right);
		log_state(coder->sim, coder->id, "has taken a dongle");
		take_dongle(coder, coder->left);
	}
	log_state(coder->sim, coder->id, "has taken a dongle");
}

static void	compile_and_rest(t_coder *coder)
{
	log_state(coder->sim, coder->id, "is compiling");
	usleep(coder->sim->time_to_compile * 1000);
	pthread_mutex_lock(&coder->compile_mutex);
	coder->last_compile_ms = get_time();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->compile_mutex);
	release_dongle(coder, coder->left);
	release_dongle(coder, coder->right);
	log_state(coder->sim, coder->id, "is debugging");
	usleep(coder->sim->time_to_debug * 1000);
	log_state(coder->sim, coder->id, "is refactoring");
	usleep(coder->sim->time_to_refactor * 1000);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	coder->last_compile_ms = get_time();
	if (coder->sim->nb_coders == 1)
	{
		while (!is_sim_over(coder->sim))
			usleep(1000);
		return (NULL);
	}
	while (!is_sim_over(coder->sim))
	{
		take_dongles(coder);
		if (is_sim_over(coder->sim))
			break ;
		compile_and_rest(coder);
	}
	return (NULL);
}
