/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:47:40 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:47:44 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

static t_fork	*phil_right_fork(t_phil *phil, t_fork *fork, t_args *args)
{
	int	index;

	index = phil->phil_id - 1;
	if (index == 0)
		return (&fork[args->num_of_philos - 1]);
	return (&fork[index - 1]);
}

static t_fork	*phil_left_fork(t_phil *phil, t_fork *fork)
{
	int	index;

	index = phil->phil_id - 1;
	return (&fork[index]);
}

static void	phil_release_both_forks(t_phil *phil, t_fork *fork, t_args *args)
{
	t_fork	*left_fork;
	t_fork	*right_fork;

	left_fork = phil_left_fork(phil, fork);
	right_fork = phil_right_fork(phil, fork, args);
	pthread_mutex_unlock(&left_fork->mutex);
	pthread_mutex_unlock(&right_fork->mutex);
}

static bool	phil_get_access_both_forks(t_phil *phil, t_fork *fork, t_args *args)
{
	t_fork	*left_fork;
	t_fork	*right_fork;
	t_fork	*first_fork;
	t_fork	*second_fork;

	left_fork = phil_left_fork(phil, fork);
	right_fork = phil_right_fork(phil, fork, args);
	if (left_fork->fork_id < right_fork->fork_id) 
		first_fork = left_fork;
	else 
		first_fork = right_fork;
	if (left_fork->fork_id < right_fork->fork_id) 
		second_fork = right_fork;
	else
		second_fork = left_fork;
	pthread_mutex_lock(&first_fork->mutex);
	if (check_stop_dinner(args) || check_philosopher_death(phil, args))
	{
		pthread_mutex_unlock(&first_fork->mutex);
		return (false);
	}
	print_state(phil->phil_id, "has taken a fork", args);
	pthread_mutex_lock(&second_fork->mutex);
	print_state(phil->phil_id, "has taken a fork", args);
	return (true);
}

bool	phil_eat(t_phil *phil, t_fork *fork, t_args *args)
{
	if (!phil_get_access_both_forks(phil, fork, args))
		return (false);
	pthread_mutex_lock(&phil->mutex);
	phil->last_meal_time = timestamp(args);
	print_state(phil->phil_id, "is eating", args);
	phil->eat_count++;
	ft_usleep(args->time_to_eat, args);
	pthread_mutex_unlock(&phil->mutex);
	phil_release_both_forks(phil, fork, args);
	return (true);
}
