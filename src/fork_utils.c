/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 16:29:05 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/27 16:29:06 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

void	decide_fork_order(t_fork *left_fork, t_fork *right_fork, \
							t_fork **first_fork, t_fork **second_fork)
{
	if (left_fork->fork_id < right_fork->fork_id)
	{
		*first_fork = left_fork;
		*second_fork = right_fork;
	}
	else
	{
		*first_fork = right_fork;
		*second_fork = left_fork;
	}
}

bool	lock_forks_and_check(t_phil *phil, t_fork *left_fork, \
		t_fork *right_fork, t_args *args)
{
	t_fork	*first_fork;
	t_fork	*second_fork;

	decide_fork_order(left_fork, right_fork, &first_fork, &second_fork);
	pthread_mutex_lock(&first_fork->mutex);
	if (check_stop_dinner(args) || check_philosopher_death(phil, args))
	{
		pthread_mutex_unlock(&first_fork->mutex);
		return (false);
	}
	pthread_mutex_lock(&second_fork->mutex);
	if (check_stop_dinner(args) || check_philosopher_death(phil, args))
	{
		pthread_mutex_unlock(&first_fork->mutex);
		pthread_mutex_unlock(&second_fork->mutex);
		return (false);
	}
	return (true);
}
