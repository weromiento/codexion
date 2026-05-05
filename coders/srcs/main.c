/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 13:28:07 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/05 14:44:59 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	main(int ac, char **av)
{
	t_sim	*sim;

	sim = init_t_sim(ac, av);
	if (!sim)
		return (1);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	init_coders(sim);
	if (init_dongles(sim) == 1)
		return (1);
	return (0);
}
