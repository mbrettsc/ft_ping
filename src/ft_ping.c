/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:40 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 15:47:15 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

struct s_ping g_ping = {0};

static void sig_handler(int signo)
{
    (void)signo;
    print_statistics();
    free_all();
    exit(EXIT_SUCCESS);
}

static void allocate_memory_for_structs()
{
    g_ping._rtt = malloc(sizeof(struct s_rtt));
    g_ping._options = malloc(sizeof(struct s_options));
    g_ping._time = malloc(sizeof(struct s_time));
}

static void check_memory_allocation()
{
    if (!g_ping._rtt || !g_ping._options || !g_ping._time) {
        exit_error("failed to allocate memory");
    }
}

static void initialize_rtt()
{
    g_ping._rtt->min_rtt = DBL_MAX;
    g_ping._rtt->max_rtt = 0.0;
    g_ping._rtt->total_rtt = 0.0;
    g_ping._rtt->total_rtt_squared = 0.0;
    g_ping._rtt->count = 0;
    g_ping._rtt->total_time_ms = 0;
}

static void initialize_options()
{
    g_ping._options->ttl = 64;
    g_ping._options->count = 0;
    g_ping._options->verbose = 0;
    g_ping._options->flood = 0;
    g_ping._options->noreverse = 0;
    g_ping._options->preload = 0;
}

static void initialize_time()
{
    memset(&g_ping._time->start_time, 0, sizeof(struct timeval));
    memset(&g_ping._time->end_time, 0, sizeof(struct timeval));
    memset(&g_ping._time->send_time, 0, sizeof(struct timeval));
}

static void init_ping()
{
    g_ping._sockfd = 0;
    g_ping._host = NULL;
    g_ping._ip = NULL;
    g_ping._dns = NULL;
    g_ping._packets_sent = 0;
    g_ping._packets_received = 0;

    allocate_memory_for_structs();
    check_memory_allocation();

    initialize_rtt();
    initialize_options();
    initialize_time();
}

int main(int ac, char **av)
{
    init_ping();
    parse_options(ac, av);
    dns_lookup();
    signal(SIGINT, sig_handler);
    icmp_loop();
    free_all();
    return 0;
}
