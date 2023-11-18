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

bool	check_stop_dinner(t_args *args)
{
	bool	stop;

	pthread_mutex_lock(&args->stop_dinner_mutex);
	stop = args->stop_dinner;
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	return (stop);
}

bool	check_philosopher_death(t_phil *phil, t_args *args)
{
	bool	should_die;

	pthread_mutex_lock(&args->stop_dinner_mutex);

	long long current_timestamp = timestamp(args);
    long long time_since_last_meal = current_timestamp - phil->last_meal_time;
	 // Debugging print statements
    printf("Checking death for philosopher %d\n", phil->phil_id);
    printf("Current timestamp: %lld\n", current_timestamp);
    printf("Last meal time: %lld\n", phil->last_meal_time);
    printf("Time since last meal: %lld\n", time_since_last_meal);
    printf("Time to die threshold: %d\n", args->time_to_die);

	should_die = (timestamp(args) - phil->last_meal_time > args->time_to_die) \
		|| (args->num_of_times_each_philosopher_must_eat == phil->eat_count \
		&& args->num_of_times_each_philosopher_must_eat_bool);
	if (should_die && !args->stop_dinner)
	{
		args->stop_dinner = 1;
		print_state(phil->phil_id, "died", args);
	}
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	return (should_die);
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
		if (check_stop_dinner(args) || check_philosopher_death(phil, args))
			return (NULL);
		phil_eat(phil, fork, args);
		print_state(phil->phil_id, "is sleeping", args);
		ft_usleep(args->time_to_sleep, args);
		print_state(phil->phil_id, "is thinking", args);
		usleep(10750);
	}
	return (NULL);
}
