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

static t_fork	*phil_get_right_fork(phil_t *phil, t_fork *fork, t_arguments *args)
{
	int index;

	index = phil->phil_id - 1;
	if (index == 0)
		return &fork[args->number_of_philosophers - 1];
	return &fork[index - 1];
}

static t_fork	*phil_get_left_fork(phil_t *phil, t_fork *fork)
{
	int index = phil->phil_id - 1;
	return &fork[index];
}

static void	philosopher_release_both_forks(phil_t *phil, t_fork *fork, t_arguments *args)
{
	t_fork *left_fork;
	t_fork *right_fork;

	left_fork = phil_get_left_fork(phil, fork);
	right_fork = phil_get_right_fork(phil, fork, args);
	pthread_mutex_unlock(&left_fork->mutex);
	pthread_mutex_unlock(&right_fork->mutex);
}

static void	philosopher_get_access_both_forks(phil_t *phil, t_fork *fork, t_arguments *args)
{
	t_fork *left_fork = phil_get_left_fork(phil, fork);
	t_fork *right_fork = phil_get_right_fork(phil, fork, args);

	// Ensure that first_fork is the one with the lower ID
	t_fork *first_fork = (left_fork->fork_id < right_fork->fork_id) ? left_fork : right_fork;
	t_fork *second_fork = (left_fork->fork_id < right_fork->fork_id) ? right_fork : left_fork;

	pthread_mutex_lock(&first_fork->mutex);
	print_state(phil->phil_id, "has taken a fork", args);
	pthread_mutex_lock(&second_fork->mutex);
	print_state(phil->phil_id, "has taken a fork", args);
}

void	phil_eat(phil_t *phil, t_fork *fork, t_arguments *args)
{
	philosopher_get_access_both_forks(phil, fork, args);
	pthread_mutex_lock(&phil->mutex);
	phil->last_meal_time = timestamp(args);
	print_state(phil->phil_id, "is eating", args);
	phil->eat_count++;
	ft_usleep(args->time_to_eat, args);
	pthread_mutex_unlock(&phil->mutex);
	philosopher_release_both_forks(phil, fork, args);
}
