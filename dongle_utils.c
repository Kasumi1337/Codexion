/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/31 14:51:29 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_both_dongles(t_simulation *sim, t_coder *me)
{
	pthread_mutex_lock(&me->mutex);
	me->ready_since = get_current_time(sim);
	pthread_mutex_unlock(&me->mutex);
	if (me->number % 2 == 1)
	{
		take_dongle(sim, me, me->left_dongle);
		take_dongle(sim, me, me->right_dongle);
	}
	else
	{
		take_dongle(sim, me, me->right_dongle);
		take_dongle(sim, me, me->left_dongle);
	}
}

void	release_dongle(t_simulation *sim, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_taken = 0;
	dongle->released_at = get_current_time(sim);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
