/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 14:42:25 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/06 14:20:52 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h" 

static int	alloc_and_check_sim(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->dongles)
	{
		free(sim->coders);
		return (1);
	}
	sim->threads = malloc(sizeof(pthread_t) * sim->nb_coders);
	if (!sim->threads)
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	return (0);
}

t_sim	*init_sim(int ac, char **av)
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	if (parse_sim_args(sim, ac, av) == 1)
	{
		free(sim);
		return (NULL);
	}
	sim->start_ms = get_time();
	if (alloc_and_check_sim(sim) == 1)
	{
		free(sim);
		return (NULL);
	}
	return (sim);
}

void	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		pthread_mutex_init(&sim->coders[i].compile_mutex, NULL);
		sim->coders[i].burned_out = 0;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->nb_coders];
		sim->coders[i].sim = sim;
		i++;
	}
}

void	cleanup(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		if (sim->dongles[i].queue)
		{
			if (sim->dongles[i].queue->data)
				free(sim->dongles[i].queue->data);
			free(sim->dongles[i].queue);
		}
		i++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	if (sim->coders)
		free(sim->coders);
	if (sim->dongles)
		free(sim->dongles);
	if (sim->threads)
		free(sim->threads);
	free(sim);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].available_at = 0;
		sim->dongles[i].held = 0;
		sim->dongles[i].queue = NULL;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (cleanup(sim, i), 1);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (cleanup(sim, i + 1), 1);
		sim->dongles[i].queue = init_pqueue(sim->scheduler);
		if (!sim->dongles[i].queue)
			return (cleanup(sim, i + 1), 1);
		i++;
	}
	return (0);
}
