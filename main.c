/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 12:27:23 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_args(int ac, char **av, t_config *config)
{
	unsigned int	i;

	if (number_of_parameters(ac))
		return (0);
	i = 1;
	while (i <= 7)
	{
		if (!is_valid_positive_number(av[i], i))
			return (0);
		set_value(config, av[i], i);
		i++;
	}
	if (!is_fifo_or_edf(av[8]))
	{
		printf("/!\\ Parsing Error /!\\\n");
		return (0);
	}
	set_value(config, av[8], 8);
	return (check_value(config));
}

static void	abort_threads(t_simulation *sim, unsigned int created,
		pthread_t monitor)
{
	set_stop(sim);
	pthread_join(monitor, NULL);
	while (created > 0)
	{
		created--;
		pthread_join(sim->coders[created].self, NULL);
	}
}

int	launch_threads(t_simulation *sim, t_thread_arg *args,
		pthread_t *monitor)
{
	unsigned int	i;

	if (pthread_create(monitor, NULL, monitor_routine, sim) != 0)
		return (0);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		args[i].sim = sim;
		args[i].coder = &sim->coders[i];
		if (pthread_create(&sim->coders[i].self, NULL, coder_routine,
				&args[i]) != 0)
		{
			abort_threads(sim, i, *monitor);
			return (0);
		}
		i++;
	}
	return (1);
}

void	join_threads(t_simulation *sim, pthread_t monitor)
{
	unsigned int	i;

	pthread_join(monitor, NULL);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(sim->coders[i].self, NULL);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_config		config;
	t_simulation	sim;
	t_thread_arg	*args;
	pthread_t		monitor;

	if (!parse_args(ac, av, &config))
		return (1);
	if (!init_simulation(&sim, &config))
	{
		printf("/!\\ Init Error /!\\\n");
		return (1);
	}
	args = malloc(sizeof(t_thread_arg) * config.number_of_coders);
	if (args == NULL || !launch_threads(&sim, args, &monitor))
	{
		printf("/!\\ Thread Error /!\\\n");
		free(args);
		cleanup(&sim);
		return (1);
	}
	join_threads(&sim, monitor);
	free(args);
	cleanup(&sim);
	return (0);
}
