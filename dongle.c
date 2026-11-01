/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long unsigned	get_key(t_simulation *sim, t_coder *me)
{
	if (sim->config.scheduler == FIFO)
		return (me->ready_since);
	return (me->when_last_compile + sim->config.time_to_bo);
}

static int	is_my_turn(t_simulation *sim, t_dongle *d, int coder_num)
{
	if (d->wait_heap.size == 0
		|| d->wait_heap.data[0].coder_num != coder_num)
		return (0);
	if (d->is_taken)
		return (0);
	if (d->never_used)
		return (1);
	return (get_current_time(sim) - d->released_at >= sim->config.dongle_cd);
}

static void	set_timeout(t_simulation *sim, t_dongle *d, struct timespec *ts)
{
	long long unsigned	abs_ms;

	abs_ms = sim->t0 + d->released_at + sim->config.dongle_cd;
	ts->tv_sec = (time_t)(abs_ms / 1000);
	ts->tv_nsec = (long)((abs_ms % 1000) * 1000000);
}

static void	do_wait(t_simulation *sim, t_dongle *d, int coder_num)
{
	struct timespec		ts;
	int					at_top;
	int					need_cd;

	at_top = d->wait_heap.size > 0
		&& d->wait_heap.data[0].coder_num == coder_num;
	need_cd = at_top && !d->is_taken && !d->never_used
		&& get_current_time(sim) - d->released_at < sim->config.dongle_cd;
	if (need_cd)
	{
		set_timeout(sim, d, &ts);
		pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
	}
	else
		pthread_cond_wait(&d->cond, &d->mutex);
}

void	take_dongle(t_simulation *sim, t_coder *me, t_dongle *dongle)
{
	t_heap_entry	e;

	e.coder_num = me->number;
	e.key = get_key(sim, me);
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->wait_heap, e);
	while (!is_my_turn(sim, dongle, me->number) && !should_stop(sim))
		do_wait(sim, dongle, me->number);
	if (!should_stop(sim))
	{
		dongle->is_taken = 1;
		dongle->never_used = 0;
		heap_pop(&dongle->wait_heap);
	}
	else
		heap_remove(&dongle->wait_heap, me->number);
	pthread_mutex_unlock(&dongle->mutex);
	if (!should_stop(sim))
		log_state(sim, me, "has taken a dongle");
}
