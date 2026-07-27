/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:30:09 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/22 14:48:50 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

t_pqueue	*init_pqueue(t_scheduler scheduler)
{
	t_pqueue	*queue;

	queue = malloc(sizeof(t_pqueue));
	if (!queue)
		return (NULL);
	queue->capacity = 4;
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

int	pqueue_push(t_pqueue *queue, t_request req)
{
	queue->data[queue->size] = req;
	queue->size++;
	sift_up(queue);
	return (0);
}

t_request	pqueue_pop(t_pqueue *queue)
{
	t_request	result;

	if (queue->size == 0)
	{
		result.coder_id = -1;
		return (result);
	}
	result = queue->data[0];
	queue->data[0] = queue->data[queue->size - 1];
	queue->size--;
	sift_down(queue);
	return (result);
}

t_request	pqueue_peek(t_pqueue *queue)
{
	t_request	res;

	if (queue->size == 0)
	{
		res.coder_id = -1;
		return (res);
	}
	return (queue->data[0]);
}
