/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:28:07 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/22 15:20:37 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	start_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			cleanup(sim, i);
			return (1);
		}
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
	{
		cleanup(sim, i);
		return (1);
	}
	return (0);
}

static void	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int ac, char **av)
{
	t_sim	*sim;

	sim = init_sim(ac, av);
	if (!sim)
		return (1);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	init_coders(sim);
	if (init_dongles(sim) == 1)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->state_mutex);
		return (1);
	}
	if (start_threads(sim) != 0)
		return (1);
	join_threads(sim);
	cleanup(sim, sim->nb_coders);
	return (0);
}
