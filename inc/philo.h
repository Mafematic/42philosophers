#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <pthread.h>
# include <stdbool.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>
# include <stdlib.h>

typedef struct s_arguments
{
	int number_of_philosophers;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	int number_of_times_each_philosopher_must_eat;
	bool number_of_times_each_philosopher_must_eat_bool;
	long long start_time;
	int have_started;
	pthread_mutex_t have_started_mutex;
	int eat_count;
	pthread_mutex_t eat_count_mutex;
	int stop_dinner;
	pthread_mutex_t stop_dinner_mutex;
} t_arguments;

typedef struct phil_
{
	int phil_id;
	pthread_t thread_handle;
	int eat_count;
	long long last_meal_time;
	pthread_mutex_t mutex;
} phil_t;

typedef struct fork_
{
	int fork_id;
	pthread_mutex_t mutex;
} t_fork;

typedef struct
{
	phil_t *phil;
	t_fork *fork;
	t_arguments *args;
} t_philosopher_args;

bool parse_arguments(int argc, char **argv, t_arguments *args);

#endif