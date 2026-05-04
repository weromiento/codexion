/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 14:42:25 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/04 15:31:41 by romgutie         ###   ########.fr       */
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

t_sim	*init_t_sim(int ac, char **av)
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	if (init_sim(sim, ac, av) == 1)
	{
		free(sim);
		return (NULL);
	}
	sim->start_ms = get_time();
	if (alloc_and_check_sim(sim) == 1)
		return (NULL);
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
		sim->coders[i].last_compile_ms = 0;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->nb_coders];
		sim->coders[i].sim = sim;
		i++;
	}
}

static void	cleanup(t_sim *sim, int nb_dongles_init)
{
	int	i;

	i = 0;
	while (i < nb_dongles_init)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		if (sim->dongles[i].queue)
		{
			free(sim->dongles[i].queue->data);
			free(sim->dongles[i].queue);
		}
		i++;
	}
	free(sim->coders);
	free(sim->dongles);
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
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].available_at = 0;
		sim->dongles[i].queue = malloc(sizeof(t_pqueue));
		if (!sim->dongles[i].queue)
		{
			cleanup(sim, i);
			return (1);
		}
		sim->dongles[i].queue->data = malloc(sizeof(t_request) * 16);
		if (!sim->dongles[i].queue->data)
		{
			free(sim->dongles[i].queue);
			sim->dongles[i].queue = NULL;
			cleanup(sim, i);
			return (1);
		}
		sim->dongles[i].queue->size = 0;
		sim->dongles[i].queue->capacity = 16;
		sim->dongles[i].queue->scheduler = sim->scheduler;
		i++;
	}
	return (0);
}
