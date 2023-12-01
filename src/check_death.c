/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_death.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 20:16:52 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/18 20:16:56 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

bool	check_stop_dinner(t_args *args)
{
	bool	stop;

	pthread_mutex_lock(&args->stop_dinner_mutex);
	stop = args->stop_dinner;
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	return (stop);
}

static bool	check_all_philos_eaten(t_args *args)
{
	bool	all_philos_eaten;

	pthread_mutex_lock(&args->eaten_mutex);
	all_philos_eaten = (args->phil_eaten == args->num_of_philos);
	pthread_mutex_unlock(&args->eaten_mutex);
	if (all_philos_eaten)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		if (!args->stop_dinner)
		{
			args->stop_dinner = 1;
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			pthread_mutex_lock(&args->print_mutex);
			printf("\033[0;32mAll philos have eaten.\033[0m\n");
			pthread_mutex_unlock(&args->print_mutex);
			return (true);
		}
		pthread_mutex_unlock(&args->stop_dinner_mutex);
	}
	return (false);
}

bool	check_philosopher_death(t_phil *phil, t_args *args)
{
	bool	stop_dinner_flag;
	bool	should_die;

	if (check_all_philos_eaten(args))
		return (true);
	pthread_mutex_lock(&args->stop_dinner_mutex);
	stop_dinner_flag = args->stop_dinner;
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	pthread_mutex_lock(&phil->death_mutex);
	should_die = (timestamp(args) - phil->last_meal_time > args->time_to_die) \
				|| args->num_of_philos == 1;
	pthread_mutex_unlock(&phil->death_mutex);
	if (should_die && !stop_dinner_flag)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		args->stop_dinner = 1;
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		print_state(phil->phil_id, "\033[0;31mdied\033[0m", args);
	}
	return (should_die);
}
