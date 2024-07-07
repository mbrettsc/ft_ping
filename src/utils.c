/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 12:18:08 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 15:49:13 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void exit_error(char *err_msg)
{
    free_all();
    fprintf(stderr, "ft_ping: error: %s", err_msg);
    exit(EXIT_FAILURE);
}

void print_usage()
{
    printf("Usage: ft_ping [-h] [-v] [-t ttl] [-f] [-n] [-l preload] [-c count] [hostname]\n");
    free_all();
    exit(EXIT_SUCCESS);
}

void free_all(void)
{
    if (g_ping._dns != NULL) {
        free(g_ping._dns);
        g_ping._dns = NULL;
    }
    if (g_ping._rtt != NULL) {
        free(g_ping._rtt);
        g_ping._rtt = NULL;
    }
    if (g_ping._options != NULL) {
        free(g_ping._options);
        g_ping._options = NULL;
    }
    if (g_ping._time != NULL) {
        free(g_ping._time);
        g_ping._time = NULL;
    }
}
