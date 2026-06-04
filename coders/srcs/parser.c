/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 23:42:38 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/04 14:21:22 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	args_is_ok(int ac, char **av)
{
	int	i;
	int	j;

	if (ac != 9)
		return (0);
	if (strcmp(av[8], "edf") != 0 && strcmp(av[8], "fifo") != 0)
		return (0);
	i = 1;
	while (i < 8)
	{
		j = 0;
		if (!av[i][j])
			return (0);
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

static int	parse_int(char *s)
{
	int		i;
	long	n;

	i = 0;
	n = 0;
	if (!s || !s[0])
		return (-1);
	while (s[i])
	{
		n = n * 10 + (s[i] - '0');
		if (n > 2147483647)
			return (-1);
		i++;
	}
	return ((int)n);
}

int	parse_sim_args(t_sim *sim, int ac, char **av)
{
	if (!args_is_ok(ac, av))
		return (fprintf(stderr, "Invalid arguments\n"), 1);
	sim->nb_coders = parse_int(av[1]);
	sim->time_to_burnout = parse_int(av[2]);
	sim->time_to_compile = parse_int(av[3]);
	sim->time_to_debug = parse_int(av[4]);
	sim->time_to_refactor = parse_int(av[5]);
	sim->nb_compiles_required = parse_int(av[6]);
	sim->dongle_cooldown = parse_int(av[7]);
	if (sim->nb_coders < 0 || sim->time_to_burnout < 0
		|| sim->time_to_compile < 0 || sim->time_to_debug < 0
		|| sim->time_to_refactor < 0 || sim->nb_compiles_required < 0
		|| sim->dongle_cooldown < 0 || sim->nb_coders == 0)
		return (1);
	if (strcmp(av[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(av[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (1);
	sim->simulation_over = 0;
	return (0);
}
