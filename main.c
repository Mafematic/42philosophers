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
} phil_t;

typedef struct spoon_
{
	int spoon_id;
	bool is_used;
	phil_t *phil;
	pthread_mutex_t mutex;
	pthread_cond_t cv;
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

	left_spoon = phil_get_left_spoon(phil, spoon);
	right_spoon = phil_get_right_spoon(phil, spoon, args);

	assert(left_spoon->phil == phil);
	assert(right_spoon->phil == phil);
	assert(left_spoon->is_used == true);
	assert(right_spoon->is_used == true);
	phil->eat_count++; // this will be optional argument to check

	printf("Phil %d eats with spoon [%d %d] for %d times\n",
		   phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id, phil->eat_count);
	// sleep(1); // argument time_to_eat
	usleep(args->time_to_eat * 1000);

	if (phil->eat_count == args->number_of_times_each_philosopher_must_eat && args->number_of_times_each_philosopher_must_eat_bool == 1)
	{
		//
	}
}

void philosopher_release_both_spoons(phil_t *phil, spoon_t *spoon, t_arguments *args)
{
	spoon_t *left_spoon;
	spoon_t *right_spoon;

	left_spoon = phil_get_left_spoon(phil, spoon);
	right_spoon = phil_get_right_spoon(phil, spoon, args);

	pthread_mutex_lock(&left_spoon->mutex);
	pthread_mutex_lock(&right_spoon->mutex);

	assert(left_spoon->phil == phil);
	assert(left_spoon->is_used == true);

	assert(right_spoon->phil == phil);
	assert(right_spoon->is_used == true);

	printf("Phil %d releasing the left spoon %d\n", phil->phil_id, left_spoon->spoon_id);
	left_spoon->phil = NULL;
	left_spoon->is_used = false;

	pthread_cond_signal(&left_spoon->cv);
	printf("phil %d signalled the phil waiting for left spoon %d\n",
		   phil->phil_id, left_spoon->spoon_id);

	pthread_mutex_unlock(&left_spoon->mutex);

	printf("Phil %d releasing the right spoon %d\n", phil->phil_id, right_spoon->spoon_id);
	right_spoon->phil = NULL;
	right_spoon->is_used = false;

	pthread_cond_signal(&right_spoon->cv);
	printf("phil %d signalled the phil waiting for right spoon %d\n",
		   phil->phil_id, right_spoon->spoon_id);

	pthread_mutex_unlock(&right_spoon->mutex);
}

bool philosopher_get_access_both_spoons(phil_t *phil, spoon_t *spoon, t_arguments *args)
{

	spoon_t *left_spoon = phil_get_left_spoon(phil, spoon);
	spoon_t *right_spoon = phil_get_right_spoon(phil, spoon, args);

	/*  before checking statys of the spoon, lock it, While one
	 *  philosopher is insepcting the state of the spoon, no
	 *  other phil must change it */
	printf("Phil %d waiting for lock on left spoon %d\n",
		   phil->phil_id, left_spoon->spoon_id);
	pthread_mutex_lock(&left_spoon->mutex);
	printf("phil %d inspecting left spoon %d state\n",
		   phil->phil_id, left_spoon->spoon_id);

	/* Case 1 : if spoon is being used by some other phil, then wait */
	while (left_spoon->is_used &&
		   left_spoon->phil != phil)
	{

		printf("phil %d blocks as left spoon %d is not available\n",
			   phil->phil_id, left_spoon->spoon_id);
		pthread_cond_wait(&left_spoon->cv, &left_spoon->mutex);

		printf("phil %d recvs signal to grab spoon %d\n",
			   phil->phil_id, left_spoon->spoon_id);
	}

	/* Case 2 : if spoon is available, grab it and try for another spoon */

	printf("phil %d finds left spoon %d available, trying to grab it\n",
		   phil->phil_id, left_spoon->spoon_id);
	left_spoon->is_used = true;
	left_spoon->phil = phil;
	printf("phil %d has successfully grabbed the left spoon %d\n",
		   phil->phil_id, left_spoon->spoon_id);
	pthread_mutex_unlock(&left_spoon->mutex);

	/* case 2.1 : Trying to grab the right spoon now*/
	printf("phil %d now making an attempt to grab the right spoon %d\n",
		   phil->phil_id, right_spoon->spoon_id);

	/* lock the right spoon before inspecting its state */
	printf("phil %d waiting for lock on right spoon %d\n",
		   phil->phil_id, right_spoon->spoon_id);
	pthread_mutex_lock(&right_spoon->mutex);
	printf("phil %d inspecting right spoon %d state\n",
		   phil->phil_id, right_spoon->spoon_id);

	if (right_spoon->is_used == false)
	{
		/* right spoon is also available, grab it and eat */
		right_spoon->is_used = true;
		right_spoon->phil = phil;
		pthread_mutex_unlock(&right_spoon->mutex);
		return true;
	}

	else if (right_spoon->is_used == true)
	{

		if (right_spoon->phil != phil)
		{

			printf("phil %d finds right spoon %d is already used by phil %d"
				   " releasing the left spoon as well\n",
				   phil->phil_id, right_spoon->spoon_id, right_spoon->phil->phil_id);

			pthread_mutex_lock(&left_spoon->mutex);
			assert(left_spoon->is_used == true);
			assert(left_spoon->phil == phil);
			left_spoon->is_used = false;
			left_spoon->phil = NULL;
			printf("phil %d release the left spoon %d\n",
				   phil->phil_id, left_spoon->spoon_id);
			pthread_mutex_unlock(&left_spoon->mutex);
			pthread_mutex_unlock(&right_spoon->mutex);
			return false;
		}
		else
		{
			printf("phil %d already has right spoon %d in hand\n",
				   phil->phil_id, right_spoon->spoon_id);
			pthread_mutex_unlock(&right_spoon->mutex);
			return true;
		}
	}
	assert(0);	  /* should be Dead code */
	return false; /*  make compiler happy */
}

void *philosopher_fn(void *arg)
{
	t_philosopher_args *phil_args = (t_philosopher_args *)arg;
	phil_t *phil = phil_args->phil;
	spoon_t *spoon = phil_args->spoon;
	t_arguments *args = phil_args->args;

	while (1)
	{
		if (philosopher_get_access_both_spoons(phil, spoon, args))
		{
			print_state(phil->phil_id, "has taken a fork");
			phil_eat(phil, spoon, args);
			print_state(phil->phil_id, "is eating");
			philosopher_release_both_spoons(phil, spoon, args);
			print_state(phil->phil_id, "is sleeping");
			usleep(args->time_to_sleep * 1000);
		}
	}
}

int main(int argc, char **argv)
{
	int i;
	pthread_attr_t attr;
	t_arguments args;

	if (!parse_arguments(argc, argv, &args))
	{
		return 1;
	}

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
		pthread_cond_init(&spoon[i].cv, NULL);
		phil[i].phil_id = i;
		phil[i].eat_count = 0;
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
