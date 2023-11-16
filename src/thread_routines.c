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
	t_arguments	*args;

	args = (t_arguments *)arg;
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

void	*philosopher_fn(void *arg)
{
	t_philosopher_args	*phil_args;
	phil_t				*phil;
	t_fork				*fork;
	t_arguments			*args;

	phil_args = (t_philosopher_args *)arg;
	phil = phil_args->phil;
	fork = phil_args->fork;
	args = phil_args->args;
	if (phil->phil_id % 2 == 0)
		ft_usleep(args->time_to_eat / 10, args);
	while (1)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		if (args->stop_dinner)
		{
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		if ((timestamp(args) - phil->last_meal_time > args->time_to_die) \
			|| (args->num_of_times_each_philosopher_must_eat == phil->eat_count \
			&& args->num_of_times_each_philosopher_must_eat_bool == true))
		{
			pthread_mutex_lock(&args->stop_dinner_mutex);
			if (!args->stop_dinner)
			{
				args->stop_dinner = 1;
				print_state(phil->phil_id, "died", args);
			}
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return (NULL); 
		}
		phil_eat(phil, fork, args);
		print_state(phil->phil_id, "is sleeping", args);
		ft_usleep(args->time_to_sleep, args);
		print_state(phil->phil_id, "is thinking", args);
		usleep(10750);
	}
	return (NULL);
}
