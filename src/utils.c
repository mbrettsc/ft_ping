/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 12:18:08 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/08 13:41:09 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"


/**
 * @brief Print error message and exit
 * 
 * @param err_msg The error message to print
 */
inline void exit_error(char *err_msg)
{
    free_all();
    fprintf(stderr, "ft_ping: error: %s\n", err_msg);
    exit(EXIT_FAILURE);
}


/**
 * @brief Print usage message and exit
 */
void print_usage()
{
    printf("Usage: sudo ./ft_ping [OPTION...] HOST ...\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
    --ttl=N                specify N as time-to-live\n\
-c, --count=NUMBER         stop after sending NUMBER packets\n\
-l, --preload=NUMBER       send NUMBER packets as fast as possible before\n\
-v, --verbose              verbose output\n\
-l, --preload=NUMBER       send NUMBER packets as fast as possible before\n\
-q, --quiet                quiet output\n");
    free_all();
    exit(EXIT_SUCCESS);
}


/**
 * @brief Free all allocated memory 
 */
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
