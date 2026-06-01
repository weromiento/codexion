/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 13:10:44 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/01 13:10:44 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static void	wake_everyone(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		i;
	int		all_done;

	sim = (t_sim *)arg;
	while (!is_sim_over(sim))
	{
		usleep(1000);
		i = 0;
		while (i < sim->nb_coders)
		{
			pthread_mutex_lock(&sim->coders[i].compile_mutex);
			if (get_time() - sim->coders[i].last_compile_ms
				> sim->time_to_burnout)
			{
				pthread_mutex_unlock(
					&sim->coders[i].compile_mutex);
				log_state(sim,
					sim->coders[i].id,
					"burned out");
				pthread_mutex_lock(&sim->state_mutex);
				sim->simulation_over = 1;
				pthread_mutex_unlock(&sim->state_mutex);
				wake_everyone(sim);
				return (NULL);
			}
			pthread_mutex_unlock(
				&sim->coders[i].compile_mutex);
			i++;
		}
		all_done = 1;
		i = 0;
		while (i < sim->nb_coders)
		{
			pthread_mutex_lock(
				&sim->coders[i].compile_mutex);
			if (sim->coders[i].compile_count
				< sim->nb_compiles_required)
				all_done = 0;
			pthread_mutex_unlock(
				&sim->coders[i].compile_mutex);
			i++;
		}
		if (all_done)
		{
			pthread_mutex_lock(&sim->state_mutex);
			sim->simulation_over = 1;
			pthread_mutex_unlock(&sim->state_mutex);
			wake_everyone(sim);
			return (NULL);
		}
	}
	return (NULL);
}
