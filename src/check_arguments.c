/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_arguments.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:47:09 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:47:12 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

static int	ft_iswhitespace(char c)
{
	if ((c >= 9 && c <= 13) || c == ' ')
	{
		return (1);
	}
	return (0);
}

static long	ft_atol(const char *nptr)
{
	int		i;
	int		is_negative;
	long	num;

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
	while (nptr[i])
	{
		if (!(nptr[i] >= '0' && nptr[i] <= '9'))
			return (0);
		num *= 10;
		num += nptr[i] - '0';
		i++;
	}
	return (num * is_negative);
}

bool	print_usage(void)
{
	write(2, "Usage: ./philo <number_of_philosophers> <time_to_die> <time_to_eat> \
		<time_to_sleep> [num_of_times_each_philosopher_must_eat]\n", 128);
	return (false);
}

bool	parse_arguments(int argc, char **argv, t_args *args)
{
	if (argc < 5 || argc > 6)
		return (print_usage());
	if (ft_atol(argv[1]) <= 0 || ft_atol(argv[2]) <= 0
		|| ft_atol(argv[3]) <= 0 || ft_atol(argv[4]) <= 0)
		return (print_usage());
	args->number_of_philosophers = ft_atol(argv[1]);
	args->time_to_die = ft_atol(argv[2]);
	args->time_to_eat = ft_atol(argv[3]);
	args->time_to_sleep = ft_atol(argv[4]);
	if (argc == 6)
	{
		if (ft_atol(argv[5]) < 0)
			return (print_usage());
		args->num_of_times_each_philosopher_must_eat = ft_atol(argv[5]);
		args->num_of_times_each_philosopher_must_eat_bool = (true);
	}
	else
	{
		args->num_of_times_each_philosopher_must_eat = 0;
		args->num_of_times_each_philosopher_must_eat_bool = (false);
	}
	return (true);
}
