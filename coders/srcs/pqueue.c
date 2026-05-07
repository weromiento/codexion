/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:30:09 by romgutie          #+#    #+#             */
/*   Updated: 2026/05/07 14:34:35 by romgutie         ###   ########.fr       */
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

int	resize_pqueue(t_pqueue *queue)
{
	t_request	*new_data;
	int			i;

	if (!queue || !queue->data)
		return (1);
	new_data = malloc(sizeof(t_request) * (queue->capacity * 2));
	if (!new_data)
		return (1);
	i = 0;
	while (i < queue->size)
	{
		new_data[i] = queue->data[i];
		i++;
	}
	free(queue->data);
	queue->data = new_data;
	queue->capacity *= 2;
	return (0);
}

int	has_priority(t_request req_a, t_request req_b, t_scheduler sched)
{
	if (sched == FIFO)
	{
		if (req_a.arrival_ms < req_b.arrival_ms)
			return (1);
	}
	else if (sched == EDF)
	{
		if (req_a.deadline_ms < req_b.deadline_ms)
			return (1);
	}
	return (0);
}

void	sift_up(t_pqueue *queue)
{
	int			i;
	int			parent;
	t_request	temp_req;

	i = queue->size - 1;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(queue->data[i], queue->data[parent], queue->scheduler))
		{
			temp_req = queue->data[i];
			queue->data[i] = queue->data[parent];
			queue->data[parent] = temp_req;
		}
		else
			break ;
		i--;
	}
}

int	pqueue_push(t_pqueue *queue, t_request req)
{
	if (queue->size == queue->capacity)
		if (resize_pqueue(queue) == 1)
			return (1);
	queue->data[queue->size] = req;
	queue->size++;
	sift_up(queue);
	return (0);
}

void	sift_down(t_pqueue *queue)
{
	int			i;
	int			left;
	int			right;
	int			best;
	t_request	temp_req;

	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;
		if (left < queue->size
			&& has_priority(queue->data[left],
				queue->data[best], queue->scheduler))
			best = left;
		if (right < queue->size
			&& has_priority(queue->data[right],
				queue->data[best], queue->scheduler))
			best = right;
		if (best == i)
			break ;
		temp_req = queue->data[i];
		queue->data[i] = queue->data[best];
		queue->data[best] = temp_req;
		i = best;
	}
}
// pqueue_pop(t_pqueue *queue)  retire et retourne la requete prioritaire
// pqueue_peek(t_pqueue *queue)  regarde la tête sans retirer
// is_higher_priority -> sift_up -> push -> sift_down -> pop -—> peek.
