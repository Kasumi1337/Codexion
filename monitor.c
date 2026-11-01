/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/09/16 12:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	announce_burnout(t_simulation *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	printf("%llu %i burned out\n", get_current_time(sim), coder->number);
	pthread_mutex_unlock(&sim->log_mutex);
}

int	check_burnout(t_simulation *sim)
{
	unsigned int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		if (get_current_time(sim) - sim->coders[i].when_last_compile
			>= sim->config.time_to_bo)
		{
			pthread_mutex_unlock(&sim->coders[i].mutex);
			announce_burnout(sim, &sim->coders[i]);
			return (1);
		}
		pthread_mutex_unlock(&sim->coders[i].mutex);
		i++;
	}
	return (0);
}

int	all_coders_done(t_simulation *sim)
{
	unsigned int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		if (sim->coders[i].how_much_compile
			< sim->config.number_of_compiles_required)
		{
			pthread_mutex_unlock(&sim->coders[i].mutex);
			return (0);
		}
		pthread_mutex_unlock(&sim->coders[i].mutex);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (!should_stop(sim))
	{
		if (check_burnout(sim) || all_coders_done(sim))
		{
			set_stop(sim);
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}
