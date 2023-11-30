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
	t_phil_args	*phil_args;
	
	phil_args = (t_phil_args *)arg;
	t_args *args;
	args = phil_args->args;

	t_phil *phil;
	phil = phil_args->phil;
	while (1) {
        for (int i = 0; i < args->num_of_philos; i++) {
            if (check_philosopher_death(&phil[i], args)) {
                pthread_mutex_lock(&args->stop_dinner_mutex);
                args->stop_dinner = 1;
                pthread_mutex_unlock(&args->stop_dinner_mutex);
                return NULL; // Exit the monitor thread if any philosopher dies
            }
        }
        usleep(500); // Check periodically (e.g., every 5 milliseconds)
    }
    return NULL;
}

bool	sleeping(t_args *args, t_phil *phil)
{
	if (check_philosopher_death(phil, args))
		return (false);
	pthread_mutex_lock(&args->stop_dinner_mutex);
	if (args->stop_dinner)
	{
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		return (false);
	}
	print_state(phil->phil_id, "is sleeping", args);
	pthread_mutex_unlock(&args->stop_dinner_mutex);
	if (!ft_usleep(args->time_to_sleep, args, phil))
		return false;
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
	usleep(10);
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
		ft_usleep(args->time_to_eat / 10, args, phil);
	while (1)
	{
		if (!phil_eat(phil, fork, args))
			break ;
		if (!sleeping(args, phil))
			break ;
		if (!thinking(args, phil))
			break ;
	}
	return (NULL);
}
