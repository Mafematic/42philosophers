/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 13:19:37 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/17 13:19:40 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <pthread.h>
# include <stdbool.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>
# include <stdlib.h>

typedef struct s_args
{
	int				num_of_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_of_times_each_philosopher_must_eat;
	bool			num_of_times_each_philosopher_must_eat_bool;
	long long		start_time;
	int				have_started;
	pthread_mutex_t	have_started_mutex;
	int				phil_eaten;
	pthread_mutex_t	eaten_mutex;
	int				stop_dinner;
	pthread_mutex_t	stop_dinner_mutex;
	pthread_mutex_t	print_mutex;
	struct timeval	te;
}	t_args;

typedef struct s_phil
{
	int				phil_id;
	pthread_t		thread_handle;
	int				eat_count;
	long long		last_meal_time;
	pthread_mutex_t	mutex;
}	t_phil;

typedef struct s_fork
{
	int				fork_id;
	pthread_mutex_t	mutex;
}	t_fork;

typedef struct s_phil_args
{
	t_phil	*phil;
	t_fork	*fork;
	t_args	*args;
}	t_phil_args;

bool		check_stop_dinner(t_args *args);
bool		check_philosopher_death(t_phil *phil, t_args *args);
void		cleanup(t_phil_args *phil_args);
void		decide_fork_order(t_fork *left_fork, t_fork *right_fork, \
				t_fork **first_fork, t_fork **second_fork);
void		ft_usleep(int ms, t_args *args, t_phil *phil);
bool		lock_forks_and_check(t_phil *phil, t_fork *left_fork, \
				t_fork *right_fork, t_args *args);
void		*monitor_fn(void *arg);
bool		parse_arguments(int argc, char **argv, t_args *args);
bool		phil_eat(t_phil *phil, t_fork *fork, t_args *args);
void		*philosopher_fn(void *arg);
void		print_state(int phil_id, const char *state, t_args *args);
long long	timestamp(t_args *args);

#endif
