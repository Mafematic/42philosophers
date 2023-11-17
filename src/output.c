/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fszendzi <fszendzi@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:47:26 by fszendzi          #+#    #+#             */
/*   Updated: 2023/11/16 15:47:28 by fszendzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philo.h"

void	print_state(int phil_id, const char *state, t_arguments *args)
{
	long long	time;
	pthread_mutex_lock(&args->print_mutex); 
	time = timestamp(args);
	printf("%lld %d %s\n", time, phil_id, state);
	pthread_mutex_unlock(&args->print_mutex); 
}
