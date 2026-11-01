/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sync.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/09/16 12:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	should_stop(t_simulation *sim)
{
	int	value;

	pthread_mutex_lock(&sim->stop_mutex);
	value = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (value);
}

void	set_stop(t_simulation *sim)
{
	unsigned int	i;

	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

void	sleep_ms(t_simulation *sim, long long unsigned ms)
{
	long long unsigned	start;

	start = get_current_time(sim);
	while (get_current_time(sim) - start < ms && !should_stop(sim))
		usleep(500);
}

void	log_state(t_simulation *sim, t_coder *coder, char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (!should_stop(sim))
		printf("%llu %i %s\n", get_current_time(sim), coder->number, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}
