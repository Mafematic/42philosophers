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
	int num_of_times_each_philosopher_must_eat;
	bool num_of_times_each_philosopher_must_eat_bool;
	long long start_time;
	int have_started;
	pthread_mutex_t have_started_mutex;
	int eat_count;
	pthread_mutex_t eat_count_mutex;
	int stop_dinner;
	pthread_mutex_t stop_dinner_mutex;
	pthread_mutex_t print_mutex;
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

void ft_usleep(int ms, t_arguments *args);
void *monitor_fn(void *arg);
bool parse_arguments(int argc, char **argv, t_arguments *args);
void phil_eat(phil_t *phil, t_fork *fork, t_arguments *args);
void *philosopher_fn(void *arg);
void print_state(int phil_id, const char *state, t_arguments *args);
long long timestamp(t_arguments *args);

#endif