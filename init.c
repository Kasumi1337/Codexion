/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_one_dongle(t_dongle *d, int capacity)
{
	if (pthread_mutex_init(&d->mutex, NULL) != 0)
		return (0);
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mutex);
		return (0);
	}
	if (!heap_init(&d->wait_heap, capacity))
	{
		pthread_cond_destroy(&d->cond);
		pthread_mutex_destroy(&d->mutex);
		return (0);
	}
	d->is_taken = 0;
	d->released_at = 0;
	d->never_used = 1;
	return (1);
}

int	init_dongles(t_simulation *sim)
{
	while (sim->dongles_ready < sim->config.number_of_coders)
	{
		if (!init_one_dongle(&sim->dongles[sim->dongles_ready],
				(int)sim->config.number_of_coders))
			return (0);
		sim->dongles_ready++;
	}
	return (1);
}

int	link_dongles(t_simulation *sim)
{
	unsigned int	i;
	unsigned int	n;

	n = sim->config.number_of_coders;
	while (sim->coders_ready < n)
	{
		i = sim->coders_ready;
		sim->coders[i].right_dongle = &sim->dongles[i];
		sim->coders[i].left_dongle = &sim->dongles[(i + n - 1) % n];
		sim->coders[i].number = i + 1;
		sim->coders[i].how_much_compile = 0;
		sim->coders[i].when_last_compile = 0;
		sim->coders[i].ready_since = 0;
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
			return (0);
		sim->coders_ready++;
	}
	return (1);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	struct timeval	tv;

	sim->config = *config;
	sim->stop = 0;
	sim->dongles_ready = 0;
	sim->coders_ready = 0;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (0);
	}
	sim->coders = malloc(sizeof(t_coder) * config->number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * config->number_of_coders);
	if (!sim->coders || !sim->dongles || !init_dongles(sim)
		|| !link_dongles(sim))
	{
		cleanup(sim);
		return (0);
	}
	gettimeofday(&tv, NULL);
	sim->t0 = (long long unsigned)tv.tv_sec * 1000;
	sim->t0 += (long long unsigned)tv.tv_usec / 1000;
	return (1);
}
