/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue_heap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 14:49:25 by romgutie          #+#    #+#             */
/*   Updated: 2026/06/22 14:49:27 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	has_priority(t_request req_a, t_request req_b, t_scheduler sched)
{
	if (sched == FIFO)
		return (req_a.arrival_ms < req_b.arrival_ms);
	if (sched == EDF)
		return (req_a.deadline_ms < req_b.deadline_ms);
	return (0);
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

void	sift_up(t_pqueue *queue)
{
	int			i;
	int			parent;
	t_request	temp;

	i = queue->size - 1;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!has_priority(queue->data[i], queue->data[parent],
				queue->scheduler))
			break ;
		temp = queue->data[i];
		queue->data[i] = queue->data[parent];
		queue->data[parent] = temp;
		i = parent;
	}
}

static int	best_child(t_pqueue *queue, int i)
{
	int	left;
	int	right;
	int	best;

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
	return (best);
}

void	sift_down(t_pqueue *queue)
{
	int			i;
	int			best;
	t_request	temp;

	i = 0;
	while (1)
	{
		best = best_child(queue, i);
		if (best == i)
			break ;
		temp = queue->data[i];
		queue->data[i] = queue->data[best];
		queue->data[best] = temp;
		i = best;
	}
}
