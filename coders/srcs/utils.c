/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 22:11:55 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/02 22:27:52 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/codexion.h"

long long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	is_sim_over(t_sim *sim)
{
	int	res;

	pthread_mutex_lock(&sim->state_mutex);
	res = sim->simulation_over;
	pthread_mutex_unlock(&sim->state_mutex);
	return (res);
}
