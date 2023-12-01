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

int	join_threads(t_phil_args *phil_args, pthread_t monitor, int created)
{
	int	i;

	i = 0;
	while (i < created)
	{
		if (pthread_join(phil_args[i].phil->thread_handle, NULL) != 0)
		{
			printf("Failed to join thread for philosopher %d\n", i);
			return (1);
		}
		i++;
	}
	if (pthread_join(monitor, NULL) != 0)
	{
		printf("Failed to join monitor thread\n");
		return (1);
	}
	return (0);
}

void	init_mutexes(t_args *args)
{
	pthread_mutex_init(&args->eaten_mutex, NULL);
	args->stop_dinner = 0;
	pthread_mutex_init(&args->stop_dinner_mutex, NULL);
	pthread_mutex_init(&args->print_mutex, NULL);
}

void	init(t_phil_args *phil_args, t_args *args)
{
	t_phil	*phil;
	t_fork	*fork;
	int		i;

	phil = (t_phil *)malloc(sizeof(t_phil) * args->num_of_philos);
	fork = (t_fork *)malloc(sizeof(t_fork) * args->num_of_philos);
	gettimeofday(&args->te, NULL);
	args->start_time = args->te.tv_sec * 1000LL + args->te.tv_usec / 1000;
	args->phil_eaten = 0;
	init_mutexes(args);
	i = 0;
	while (i < args->num_of_philos)
	{
		fork[i].fork_id = i;
		pthread_mutex_init(&fork[i].mutex, NULL);
		phil[i].phil_id = i + 1;
		phil[i].eat_count = 0;
		phil[i].last_meal_time = timestamp(args);
		pthread_mutex_init(&phil[i].death_mutex, NULL);
		phil_args[i].phil = &phil[i];
		phil_args[i].fork = fork;
		phil_args[i].args = args;
		i++;
	}
}

int	create_threads(pthread_t *monitor, t_args *args, t_phil_args *phil_args)
{
	int	i;

	if (pthread_create(monitor, NULL, monitor_fn, (void *)phil_args) != 0)
	{
		printf("Failed to create monitor thread\n");
		return (1);
	}
	i = 0;
	while (i < args->num_of_philos)
	{
		if (pthread_create(&phil_args[i].phil->thread_handle, \
			NULL, philosopher_fn, (void *)&phil_args[i]) != 0)
		{
			printf("Failed to create thread for philosopher %d\n", i);
			join_threads(phil_args, *monitor, i);
			return (1);
		}
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_phil_args			*phil_args;
	t_args				args;
	pthread_t			monitor_thread;

	if (!parse_arguments(argc, argv, &args))
		return (1);
	phil_args = (t_phil_args *)malloc(sizeof(t_phil_args) * args.num_of_philos);
	if (!phil_args)
	{
		printf("Failed to allocate memory for philosophers\n");
		return (1);
	}
	init(phil_args, &args);
	if (create_threads(&monitor_thread, &args, phil_args))
	{
		cleanup(phil_args);
		return (1);
	}
	if (join_threads(phil_args, monitor_thread, args.num_of_philos))
	{
		cleanup(phil_args);
		return (1);
	}
	cleanup(phil_args);
	return (0);
}
