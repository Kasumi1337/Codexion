/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thbeil <thbeil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 11:49:21 by thbeil            #+#    #+#             */
/*   Updated: 2026/09/16 12:00:00 by thbeil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	ft_atoll(char *str)
{
	long long	res;
	int			nb;
	int			i;

	res = 0;
	i = 0;
	while (str[i])
	{
		nb = str[i] - '0';
		if (res > LLONG_MAX / 10
			|| (res == LLONG_MAX / 10 && nb > LLONG_MAX % 10))
			return (-1);
		res = res * 10 + nb;
		i++;
	}
	return (res);
}

long long unsigned	get_current_time(t_simulation *sim)
{
	struct timeval		tv;
	long long unsigned	now;

	gettimeofday(&tv, NULL);
	now = (long long unsigned)tv.tv_sec * 1000;
	now += (long long unsigned)tv.tv_usec / 1000;
	return (now - sim->t0);
}
