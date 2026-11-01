/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_free(t_heap *h)
{
	free(h->data);
	h->data = NULL;
	h->size = 0;
}

void	heap_push(t_heap *h, t_heap_entry e)
{
	int	i;

	if (h->size >= h->capacity)
		return ;
	i = h->size;
	h->data[i] = e;
	h->size++;
	heap_sift_up(h, i);
}

void	heap_pop(t_heap *h)
{
	if (h->size == 0)
		return ;
	h->size--;
	if (h->size > 0)
	{
		h->data[0] = h->data[h->size];
		heap_sift_down(h, 0);
	}
}

void	heap_remove(t_heap *h, int coder_num)
{
	int	i;

	i = 0;
	while (i < h->size && h->data[i].coder_num != coder_num)
		i++;
	if (i >= h->size)
		return ;
	h->size--;
	if (i < h->size)
	{
		h->data[i] = h->data[h->size];
		heap_sift_down(h, i);
		heap_sift_up(h, i);
	}
}
