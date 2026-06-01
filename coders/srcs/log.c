/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 13:01:14 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/01 13:01:14 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	log_state(t_sim *sim, int coder_id, char *msg)
{
	long	now;

	pthread_mutex_lock(&sim->log_mutex);
	if (!is_sim_over(sim))
	{
		now = get_time() - sim->start_ms;
		printf("%ld %d %s\n", now, coder_id, msg);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}
