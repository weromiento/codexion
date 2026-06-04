/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 11:31:07 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/04 14:30:26 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

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
		if (dongle->queue->size > 0
			&& pqueue_peek(dongle->queue).coder_id == coder->id
			&& get_time() >= dongle->available_at)
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pqueue_pop(dongle->queue);
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at = get_time()
		+ coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->nb_coders == 1)
	{
		coder->last_compile_ms = get_time();
		while (!is_sim_over(coder->sim))
		{
			usleep(1000);
		}
		return (NULL);
	}
	coder->last_compile_ms = get_time();
	while (!is_sim_over(coder->sim))
	{
		if (coder->id % 2 == 0)
		{
			take_dongle(coder, coder->left);
			log_state(coder->sim, coder->id, "has taken a dongle");
			take_dongle(coder, coder->right);
			log_state(coder->sim, coder->id, "has taken a dongle");
		}
		else
		{
			take_dongle(coder, coder->right);
			log_state(coder->sim, coder->id, "has taken a dongle");
			take_dongle(coder, coder->left);
			log_state(coder->sim, coder->id, "has taken a dongle");
		}
		if (is_sim_over(coder->sim))
			break ;
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
	return (NULL);
}
