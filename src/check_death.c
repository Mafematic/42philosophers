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

bool	check_philosopher_death(t_phil *phil, t_args *args)
{
	bool	should_die;

	pthread_mutex_lock(&args->stop_dinner_mutex);
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
