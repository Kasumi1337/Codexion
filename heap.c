/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	heap_cmp(t_heap_entry a, t_heap_entry b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.coder_num < b.coder_num);
}

static void	heap_swap(t_heap *h, int i, int j)
{
	t_heap_entry	tmp;

	tmp = h->data[i];
	h->data[i] = h->data[j];
	h->data[j] = tmp;
}

void	heap_sift_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!heap_cmp(h->data[i], h->data[parent]))
			break ;
		heap_swap(h, i, parent);
		i = parent;
	}
}

void	heap_sift_down(t_heap *h, int i)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;
		if (left < h->size && heap_cmp(h->data[left], h->data[best]))
			best = left;
		if (right < h->size && heap_cmp(h->data[right], h->data[best]))
			best = right;
		if (best == i)
			break ;
		heap_swap(h, i, best);
		i = best;
	}
}

int	heap_init(t_heap *h, int capacity)
{
	h->data = malloc(sizeof(t_heap_entry) * capacity);
	if (!h->data)
		return (0);
	h->size = 0;
	h->capacity = capacity;
	return (1);
}
