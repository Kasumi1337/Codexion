/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 12:00:00 by thbeil            #+#    #+#             */
/*   Updated: 2026/10/26 14:13:25 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	set_value(t_config *config, char *str, int index)
{
	if (index == 1)
		config->number_of_coders = ft_atoll(str);
	else if (index == 2)
		config->time_to_bo = ft_atoll(str);
	else if (index == 3)
		config->time_to_compile = ft_atoll(str);
	else if (index == 4)
		config->time_to_debug = ft_atoll(str);
	else if (index == 5)
		config->time_to_refactor = ft_atoll(str);
	else if (index == 6)
		config->number_of_compiles_required = ft_atoll(str);
	else if (index == 7)
		config->dongle_cd = ft_atoll(str);
	else if (index == 8 && strcmp(str, "fifo") == 0)
		config->scheduler = FIFO;
	else if (index == 8)
		config->scheduler = EDF;
}
