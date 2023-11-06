#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct
{
	int number_of_philosophers;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	int number_of_times_each_philosopher_must_eat;
	bool number_of_times_each_philosopher_must_eat_bool;
} t_arguments;

typedef struct phil_
{
	int phil_id;
	pthread_t thread_handle;
	int eat_count;
	long long last_meal_time;
	pthread_mutex_t mutex;
} phil_t;

typedef struct spoon_
{
	int spoon_id;
	bool is_used;
	phil_t *phil;
	pthread_mutex_t mutex;
	//pthread_cond_t cv;
} spoon_t;

typedef struct
{
	phil_t *phil;
	spoon_t *spoon;
	t_arguments *args;
} t_philosopher_args;

/* Creating Timestamps */
long long current_timestamp()
{
	struct timeval te;
	gettimeofday(&te, NULL);										 
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
	return milliseconds;
}

void print_state(int philosopher_number, const char *state)
{
	long long timestamp = current_timestamp();
	printf("%lld %d %s\n", timestamp, philosopher_number, state);
}

/*Time stamp end*/

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

spoon_t *phil_get_right_spoon(phil_t *phil, spoon_t *spoon, t_arguments *args)
{
	int phil_id;

	phil_id = phil->phil_id;
	if (phil_id == 0)
		return &spoon[args->number_of_philosophers - 1];
	return &spoon[phil_id - 1];
}

spoon_t *phil_get_left_spoon(phil_t *phil, spoon_t *spoon)
{
	return &spoon[phil->phil_id];
}

void phil_eat(phil_t *phil, spoon_t *spoon, t_arguments *args)
{
	spoon_t *left_spoon;
	spoon_t *right_spoon;

	//left_spoon = phil_get_left_spoon(phil, spoon);
	//right_spoon = phil_get_right_spoon(phil, spoon, args);

	pthread_mutex_lock(&phil->mutex);
	phil->last_meal_time = current_timestamp();
	print_state(phil->phil_id, "is eating");
	pthread_mutex_unlock(&phil->mutex);

	usleep(200 * 1000); // args->time_to_eat;
	phil->eat_count++;

	//printf("Phil %d eats with spoon [%d %d] for %d times\n",
	//	   phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id, phil->eat_count);

	//pthread_mutex_lock(&phil->mutex);
	if (phil->eat_count == args->number_of_times_each_philosopher_must_eat && args->number_of_times_each_philosopher_must_eat_bool == 1)
	{
		//
	}
	//pthread_mutex_unlock(&phil->mutex);
}

void philosopher_release_both_spoons(phil_t *phil, spoon_t *spoon, t_arguments *args)
{
	spoon_t *left_spoon;
	spoon_t *right_spoon;

	left_spoon = phil_get_left_spoon(phil, spoon);
	right_spoon = phil_get_right_spoon(phil, spoon, args);

	pthread_mutex_lock(&left_spoon->mutex);
	pthread_mutex_lock(&right_spoon->mutex);
	left_spoon->phil = NULL;
	left_spoon->is_used = false;
	pthread_mutex_unlock(&left_spoon->mutex);
	right_spoon->phil = NULL;
	right_spoon->is_used = false;
	pthread_mutex_unlock(&right_spoon->mutex);
}

bool philosopher_get_access_both_spoons(phil_t *phil, spoon_t *spoon, t_arguments *args)
{
	spoon_t *left_spoon = phil_get_left_spoon(phil, spoon);
	spoon_t *right_spoon = phil_get_right_spoon(phil, spoon, args);

	// Ensure that first_spoon is the one with the lower ID
	spoon_t *first_spoon = (left_spoon->spoon_id < right_spoon->spoon_id) ? left_spoon : right_spoon;
	spoon_t *second_spoon = (left_spoon->spoon_id < right_spoon->spoon_id) ? right_spoon : left_spoon;

	// Lock and acquire the first spoon
	pthread_mutex_lock(&first_spoon->mutex);
	while (first_spoon->is_used && first_spoon->phil != phil)
	{
		pthread_mutex_unlock(&first_spoon->mutex);
		usleep(1000); // Random delay to prevent starvation
		pthread_mutex_lock(&first_spoon->mutex);
	}
	first_spoon->is_used = true;
	first_spoon->phil = phil;
	pthread_mutex_unlock(&first_spoon->mutex);

	// Lock and try to acquire the second spoon
	pthread_mutex_lock(&second_spoon->mutex);
	if (!second_spoon->is_used)
	{
		second_spoon->is_used = true;
		second_spoon->phil = phil;
		pthread_mutex_unlock(&second_spoon->mutex);
		return true; // Successfully acquired both spoons
	}
	else
	{
		// If the second spoon is not available, release the first spoon
		pthread_mutex_lock(&first_spoon->mutex);
		first_spoon->is_used = false;
		first_spoon->phil = NULL;
		pthread_mutex_unlock(&first_spoon->mutex);
		pthread_mutex_unlock(&second_spoon->mutex);
		return false;
	}
}

void *philosopher_fn(void *arg)
{
	t_philosopher_args *phil_args = (t_philosopher_args *)arg;
	phil_t *phil = phil_args->phil;
	spoon_t *spoon = phil_args->spoon;
	t_arguments *args = phil_args->args;

	while (1)
	{
		// if (current_timestamp() - phil->last_meal_time > args->time_to_die)
		/* Please delete*/
		if (current_timestamp() - phil->last_meal_time > 410)
		{
			print_state(phil->phil_id, "Philosopher died");
			break; 
		}

		if (philosopher_get_access_both_spoons(phil, spoon, args))
		{
			print_state(phil->phil_id, "has taken a fork");
			phil_eat(phil, spoon, args);
			philosopher_release_both_spoons(phil, spoon, args);
		}
		print_state(phil->phil_id, "is sleeping");
		// usleep(args->time_to_sleep * 1000);
		/* Please delete*/
		usleep(100 * 1000);
		print_state(phil->phil_id, "is thinking");
		usleep(1000);
	}
	return NULL; 
}

//int main(int argc, char **argv)
int main(void)
{
	int i;
	pthread_attr_t attr;
	t_arguments args;

	/*if (!parse_arguments(argc, argv, &args))
	{
		return 1;
	}*/
	/* Please delete later */
	args.number_of_philosophers = 5; 

	phil_t phil[args.number_of_philosophers];
	spoon_t spoon[args.number_of_philosophers];
	t_philosopher_args phil_args[args.number_of_philosophers];

	/* Initialize all spoons */
	i = 0;
	while (i < args.number_of_philosophers)
	{
		spoon[i].spoon_id = i;
		spoon[i].is_used = false;
		spoon[i].phil = NULL;
		pthread_mutex_init(&spoon[i].mutex, NULL);
		//pthread_cond_init(&spoon[i].cv, NULL);
		phil[i].phil_id = i;
		phil[i].eat_count = 0;
		phil[i].last_meal_time = current_timestamp();
		pthread_mutex_init(&phil[i].mutex, NULL); 
		phil_args[i].phil = &phil[i];
		phil_args[i].spoon = spoon;
		phil_args[i].args = &args;
		i++;
	}
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/* Create philosopher threads */
	i = 0;
	while (i < args.number_of_philosophers)
	{
		pthread_create(&phil[i].thread_handle, &attr, philosopher_fn, &phil_args[i]);
		i++;
	}
	pthread_exit(0);
	return 0;
}

/* To Dos
Add logic for time arguments when starting the function
if (time_to_die <= (time_to_eat + time_to_sleep + buffer_time)) {

Implement stop of process, when eat_count has been reached for all


*/
