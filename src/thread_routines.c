/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_routines.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:47:53 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:47:55 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

void	*monitor_fn(void *arg)
{
	t_args	*args;

	args = (t_args *)arg;
	while (1)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		if (args->stop_dinner)
		{
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		usleep(50);
	}
	return (NULL);
}

bool	sleeping(t_args *args, t_phil *phil)
{
	pthread_mutex_lock(&args->stop_dinner_mutex);
	if (args->stop_dinner)
	{
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		return (false);
	}
	print_state(phil->phil_id, "is sleeping", args);
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	return (true);
}

bool	thinking(t_args *args, t_phil *phil)
{
	pthread_mutex_lock(&args->stop_dinner_mutex);
	if (args->stop_dinner)
	{
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		return (false);
	}
	print_state(phil->phil_id, "is thinking", args);
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	return (true);
}

void	*philosopher_fn(void *arg)
{
	t_phil_args			*phil_args;
	t_phil				*phil;
	t_fork				*fork;
	t_args				*args;

	phil_args = (t_phil_args *)arg;
	phil = phil_args->phil;
	fork = phil_args->fork;
	args = phil_args->args;
	if (phil->phil_id % 2 == 0)
		ft_usleep(args->time_to_eat / 10, args);
	while (1)
	{
		if (!phil_eat(phil, fork, args))
			break ;
		if (!sleeping(args, phil))
			break ;
		ft_usleep(args->time_to_sleep, args);
		if (!thinking(args, phil))
			break ;
		usleep(50);
	}
	return (NULL);
}
