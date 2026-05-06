/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:30:09 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/06 14:30:09 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

t_pqueue	*init_pqueue(t_scheduler scheduler)
{
	t_pqueue	*queue;

	queue = malloc(sizeof(t_pqueue));
	if (!queue)
		return (NULL);
	queue->capacity = 16;
	queue->size = 0;
	queue->scheduler = scheduler;
	queue->data = malloc(sizeof(t_request) * queue->capacity);
	if (!queue->data)
	{
		free(queue);
		return (NULL);
	}
	return (queue);
}

void	destroy_pqueue(t_pqueue *queue)
{
	if (!queue)
		return ;
	free(queue->data);
	free(queue);
}

int	resize_pqueue(t_pqueue *queue, pthread_mutex_t *mutex)
{
	t_request	*old_data;
	t_request	*new_data;
	int			i;

	if (!queue || !queue->data)
		return (1);
	pthread_mutex_lock(mutex);
	old_data = queue->data;
	new_data = malloc(sizeof(t_request) * (queue->capacity * 2));
	if (!new_data)
	{
		pthread_mutex_unlock(mutex);
		return (1);
	}
	i = 0;
	while (i < queue->size)
	{
		new_data[i] = old_data[i];
		i++;
	}
	queue->data = new_data;
	queue->capacity *= 2;
	free(old_data);
	pthread_mutex_unlock(mutex);
	return (0);
}
