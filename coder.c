/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/09/16 12:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	do_compile(t_simulation *sim, t_coder *coder)
{
	log_state(sim, coder, "is compiling");
	pthread_mutex_lock(&coder->mutex);
	coder->when_last_compile = get_current_time(sim);
	pthread_mutex_unlock(&coder->mutex);
	sleep_ms(sim, sim->config.time_to_compile);
	if (should_stop(sim))
	{
		release_dongle(sim, coder->left_dongle);
		release_dongle(sim, coder->right_dongle);
		return (0);
	}
	pthread_mutex_lock(&coder->mutex);
	coder->how_much_compile++;
	pthread_mutex_unlock(&coder->mutex);
	release_dongle(sim, coder->left_dongle);
	release_dongle(sim, coder->right_dongle);
	return (1);
}

void	do_debug_and_refactor(t_simulation *sim, t_coder *coder)
{
	log_state(sim, coder, "is debugging");
	sleep_ms(sim, sim->config.time_to_debug);
	if (should_stop(sim))
		return ;
	log_state(sim, coder, "is refactoring");
	sleep_ms(sim, sim->config.time_to_refactor);
}

void	*coder_routine(void *arg)
{
	t_thread_arg	*ta;
	t_simulation	*sim;
	t_coder			*coder;

	ta = (t_thread_arg *)arg;
	sim = ta->sim;
	coder = ta->coder;
	while (!should_stop(sim))
	{
		take_both_dongles(sim, coder);
		if (should_stop(sim))
			break ;
		if (!do_compile(sim, coder))
			break ;
		do_debug_and_refactor(sim, coder);
	}
	return (NULL);
}
