#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>
#include <stdlib.h>


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

long long timestamp(t_arguments *args)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long current_time = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
	return (current_time - args->start_time); // Subtract the start time to get the relative time
}

void print_state(int philosopher_number, const char *state, t_arguments *args)
{
	long long time = timestamp(args);
	printf("%lld %d %s\n", time, philosopher_number, state);
}

/* Checking arguments start*/
static int ft_iswhitespace(char c)
{
	if ((c >= 9 && c <= 13) || c == ' ')
	{
		return (1);
	}
	return (0);
}

long ft_atol(const char *nptr)
{
	int i;
	int is_negative;
	long num;

	i = 0;
	is_negative = 1;
	num = 0;
	while (ft_iswhitespace(nptr[i]))
		i++;
	if (nptr[i] == '+')
		i++;
	else if (nptr[i] == '-')
	{
		is_negative *= -1;
		i++;
	}
	if (!(nptr[i] >= '0' && nptr[i] <= '9'))
		return (0);
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		num *= 10;
		num += nptr[i] - '0';
		i++;
	}
	return (num * is_negative);
}

bool parse_arguments(int argc, char **argv, t_arguments *args)
{
	if (argc < 5 || argc > 6)
	{
		write(2, "Usage: ./philo <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [number_of_times_each_philosopher_must_eat]\n", 128);
		return false;
	}
	args->number_of_philosophers = ft_atol(argv[1]);
	args->time_to_die = ft_atol(argv[2]);
	args->time_to_eat = ft_atol(argv[3]);
	args->time_to_sleep = ft_atol(argv[4]);

	if (argc == 6)
	{
		args->number_of_times_each_philosopher_must_eat = ft_atol(argv[5]);
		args->number_of_times_each_philosopher_must_eat_bool = true;
	}
	else
	{
		args->number_of_times_each_philosopher_must_eat = 0;
		args->number_of_times_each_philosopher_must_eat_bool = false;
	}
	return true;
}

/*Checking Arguments end*/

t_fork *phil_get_right_fork(phil_t *phil, t_fork *fork, t_arguments *args)
{
	int index = phil->phil_id - 1;

	if (index == 0)
		return &fork[args->number_of_philosophers - 1];
	return &fork[index - 1];
}

t_fork *phil_get_left_fork(phil_t *phil, t_fork *fork)
{
	int index = phil->phil_id - 1;
	return &fork[index];
}

void philosopher_release_both_forks(phil_t *phil, t_fork *fork, t_arguments *args)
{
	t_fork *left_fork;
	t_fork *right_fork;

	left_fork = phil_get_left_fork(phil, fork);
	right_fork = phil_get_right_fork(phil, fork, args);
	pthread_mutex_unlock(&left_fork->mutex);
	pthread_mutex_unlock(&right_fork->mutex);
}

void philosopher_get_access_both_forks(phil_t *phil, t_fork *fork, t_arguments *args)
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

void ft_usleep(int ms, t_arguments *args)
{
	long int time;

	time = timestamp(args);
	while (timestamp(args) - time < ms)
		usleep(ms / 10);
}

void phil_eat(phil_t *phil, t_fork *fork, t_arguments *args)
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

void *monitor_fn(void *arg)
{
	t_arguments *args = (t_arguments *)arg;

	while (1)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		if (args->stop_dinner)
		{
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&args->stop_dinner_mutex);
		usleep(50);
	}
	return NULL;
}

int check_for_stop(t_arguments *args) {
    int stop = 0;
    pthread_mutex_lock(&args->stop_dinner_mutex);
    stop = args->stop_dinner;
    pthread_mutex_unlock(&args->stop_dinner_mutex);
    return stop;
}

void *philosopher_fn(void *arg)
{
	t_philosopher_args *phil_args = (t_philosopher_args *)arg;
	phil_t *phil = phil_args->phil;
	t_fork *fork = phil_args->fork;
	t_arguments *args = phil_args->args;

	if (phil->phil_id % 2 == 0)
		ft_usleep(args->time_to_eat / 10, args);
	while (1)
	{
		pthread_mutex_lock(&args->stop_dinner_mutex);
		if (args->stop_dinner)
		{
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&args->stop_dinner_mutex);

		if (timestamp(args) - phil->last_meal_time > args->time_to_die)
		{
			pthread_mutex_lock(&args->stop_dinner_mutex);
			if (!args->stop_dinner) // Check the flag to prevent multiple death messages
			{
				args->stop_dinner = 1;
				print_state(phil->phil_id, "died", args);
			}
			pthread_mutex_unlock(&args->stop_dinner_mutex);
			return NULL; 
		}
		phil_eat(phil, fork, args);
		
		print_state(phil->phil_id, "is sleeping", args);
		ft_usleep(args->time_to_sleep, args);
		print_state(phil->phil_id, "is thinking", args);
		usleep(5500);
	}
	return NULL;
}

// int main(int argc, char **argv)
int main(int argc, char **argv)
{
	int i;
	t_arguments args;

	if (!parse_arguments(argc, argv, &args))
	{
		return 1;
	}

	struct timeval te;
	gettimeofday(&te, NULL);
	args.start_time = te.tv_sec * 1000LL + te.tv_usec / 1000; // Record the start time before any philosopher starts
	pthread_mutex_init(&args.eat_count_mutex, NULL);
	args.stop_dinner = 0;
	pthread_mutex_init(&args.stop_dinner_mutex, NULL);
	args.have_started = 0;								// Add this flag to your t_arguments structure
	pthread_mutex_init(&args.have_started_mutex, NULL); // Add this mutex as well

	phil_t phil[args.number_of_philosophers];
	t_fork fork[args.number_of_philosophers];
	t_philosopher_args phil_args[args.number_of_philosophers];

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

	return 0;
}

/* To Dos
Add logic for time arguments when starting the function
if (time_to_die <= (time_to_eat + time_to_sleep + buffer_time)) {

Implement stop of process, when eat_count has been reached for all


*/