/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:48:02 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:48:04 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

long long	timestamp(t_args *args)
{
	struct timeval	tv;
	long long		current_time;

	gettimeofday(&tv, NULL);
	current_time = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
	return (current_time - args->start_time);
}

int	ft_usleep(int ms, t_args *args, t_phil *phil)
{
	long int	time;

	time = timestamp(args);
	while (timestamp(args) - time < ms)
	{
		usleep(10);
		if (check_philosopher_death(phil, args))
			return (0);
	}
	return (1);
}
