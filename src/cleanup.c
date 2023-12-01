/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 21:06:40 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/18 21:06:42 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

void	cleanup(t_phil_args *phil_args)
{
	int	i;

	i = 0;
	while (i < phil_args->args->num_of_philos)
	{
		pthread_mutex_destroy(&phil_args->fork[i].mutex);
		pthread_mutex_destroy(&phil_args->phil[i].death_mutex);
		i++;
	}
	pthread_mutex_destroy(&phil_args->args->eaten_mutex);
	pthread_mutex_destroy(&phil_args->args->stop_dinner_mutex);
	pthread_mutex_destroy(&phil_args->args->print_mutex);
	free(phil_args->phil);
	free(phil_args->fork);
	free(phil_args);
}
