/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue_heap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romgutie <romgutie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 14:49:25 by romgutie          #+#    #+#             */
/*   Updated: 2026/07/05 16:06:49 by romgutie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

int	has_priority(t_request req_a, t_request req_b, t_scheduler sched)
{
	if (sched == FIFO)
		return (req_a.arrival_ms < req_b.arrival_ms);
	if (sched == EDF)
	{
		if (req_a.deadline_ms != req_b.deadline_ms)
			return (req_a.deadline_ms < req_b.deadline_ms);
		return (req_a.arrival_ms < req_b.arrival_ms);
	}
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
