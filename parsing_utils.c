/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/19 00:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	number_of_parameters(int nb)
{
	if (nb != 9)
	{
		printf("You need 8 arguments\n\n");
		printf("  1 - Number of coders\n");
		printf("  2 - Time to burnout\n");
		printf("  3 - Time to compile\n");
		printf("  4 - Time to debug\n");
		printf("  5 - Time to refactor\n");
		printf("  6 - Number of compiles required\n");
		printf("  7 - Dongle Cooldown\n");
		printf("  8 - Scheduler\n");
		return (1);
	}
	return (0);
}

int	is_valid_positive_number(char *str, int index)
{
	int	i;

	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
		i++;
	if (i == 0 || str[i] != '\0' || ft_atoll(str) < 0
		|| ((index == 1 || index == 6) && ft_atoll(str) > INT_MAX))
	{
		printf("/!\\ Parsing Error /!\\\n");
		return (0);
	}
	return (1);
}

int	is_fifo_or_edf(char *str)
{
	if (strcmp(str, "edf") == 0)
		return (1);
	if (strcmp(str, "fifo") == 0)
		return (1);
	return (0);
}

int	check_value(t_config *config)
{
	if (config->number_of_coders == 0)
	{
		printf("/!\\ Parsing Error /!\\\n");
		return (0);
	}
	return (1);
}
