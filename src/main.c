/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:47:18 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:47:20 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

int main(int argc, char **argv)
{
	int i;
	t_args args;
	t_phil *phil;
	t_fork *fork;
	t_philosopher_args *phil_args;

	if (!parse_arguments(argc, argv, &args))
		return 1;

	phil = (t_phil *)malloc(sizeof(t_phil) * args.number_of_philosophers);
	fork = (t_fork *)malloc(sizeof(t_fork) * args.number_of_philosophers);
	phil_args = (t_philosopher_args *)malloc(sizeof(t_philosopher_args) * args.number_of_philosophers);
	
	gettimeofday(&args.te, NULL);
	args.start_time = args.te.tv_sec * 1000LL + args.te.tv_usec / 1000;
	pthread_mutex_init(&args.eat_count_mutex, NULL);
	args.stop_dinner = 0;
	pthread_mutex_init(&args.stop_dinner_mutex, NULL);
	args.have_started = 0;
	pthread_mutex_init(&args.have_started_mutex, NULL);
	pthread_mutex_init(&args.print_mutex, NULL);
	
	/* Initialize all forks */
	i = 0;
	while (i < args.number_of_philosophers)
	{
		fork[i].fork_id = i;
		pthread_mutex_init(&fork[i].mutex, NULL);
		phil[i].phil_id = i + 1;
		phil[i].eat_count = 0;
		phil[i].last_meal_time = timestamp(&args);
		pthread_mutex_init(&phil[i].mutex, NULL);
		phil_args[i].phil = &phil[i];
		phil_args[i].fork = fork;
		phil_args[i].args = &args;
		i++;
	}
	/* Create Monito Thread*/
	pthread_t monitor_thread;
	if (pthread_create(&monitor_thread, NULL, monitor_fn, (void*)&args) != 0)
	{
		printf("Failed to create monitor thread\n");
		return 1;
	}
	pthread_mutex_lock(&args.have_started_mutex);
	args.have_started = 1;
	pthread_mutex_unlock(&args.have_started_mutex);
	i = 0;

	while (i < args.number_of_philosophers)
	{
		if (pthread_create(&phil[i].thread_handle, NULL, philosopher_fn, (void*)&phil_args[i]) != 0)
		{
			printf("Failed to create thread for philosopher %d\n", i);
			return 1;
		}
		i++;
	}
	i = 0;
	while (i < args.number_of_philosophers)
	{
		if (pthread_join(phil[i].thread_handle, NULL) != 0)
		{
			printf("Failed to join thread for philosopher %d\n", i);
		}
		i++;
	}
	if (pthread_join(monitor_thread, NULL) != 0)
	{
		printf("Failed to join monitor thread\n");
		// Handle error
	}

	i = 0;
	while (i < args.number_of_philosophers)
	{
		pthread_mutex_destroy(&fork[i].mutex);
		pthread_mutex_destroy(&phil[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&args.eat_count_mutex);
	pthread_mutex_destroy(&args.stop_dinner_mutex);
	pthread_mutex_destroy(&args.have_started_mutex);
	pthread_mutex_destroy(&args.print_mutex);

	free(phil);
	free(fork);
	free(phil_args);

	return 0;
}