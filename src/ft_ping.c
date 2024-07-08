/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:40 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/08 14:34:57 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"


/**
 * @brief The global ping struct
*/
struct s_ping g_ping = {0};


/**
 * @brief DNS lookup for the host name
 */
static void dns_lookup(void)
{
    struct addrinfo hints, *res;
    char ip_str[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if (getaddrinfo(g_ping._host, NULL, &hints, &res) != 0) {
        exit_error("Name or service not known");
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    if (!inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str))) {
        freeaddrinfo(res);
        exit_error("Failed to convert IP address");
    }
    freeaddrinfo(res);

    g_ping._ip = strdup(ip_str);
    if (!g_ping._ip) {
        exit_error("Failed to allocate memory for IP address");
    }
}


/**
 * @brief Signal handler for SIGINT
 */
static void sig_handler(int signo)
{
    (void)signo;
    print_statistics();
    free_all();
    if (g_ping._sockfd > 0) {
        close(g_ping._sockfd);
    }
    exit(EXIT_SUCCESS);
}


/**
 * @brief Allocate memory for the structs
 */
static void allocate_memory_for_structs()
{
    g_ping._rtt = malloc(sizeof(struct s_rtt));
    g_ping._options = malloc(sizeof(struct s_options));
    g_ping._time = malloc(sizeof(struct s_time));
}


/**
 * @brief Check if memory allocation was successful
 */
static void check_memory_allocation()
{
    if (!g_ping._rtt || !g_ping._options || !g_ping._time) {
        exit_error("failed to allocate memory");
    }
}


/**
 * @brief Initialize the round trip time struct
 */
static void initialize_rtt()
{
    g_ping._rtt->min_rtt = DBL_MAX;
    g_ping._rtt->max_rtt = 0.0;
    g_ping._rtt->total_rtt = 0.0;
    g_ping._rtt->total_rtt_squared = 0.0;
    g_ping._rtt->count = 0;
    g_ping._rtt->total_time_ms = 0;
}


/**
 * @brief Initialize the options struct 
 */
static void initialize_options()
{
    g_ping._options->ttl = 64;
    g_ping._options->count = -1;
    g_ping._options->verbose = -1;
    g_ping._options->timeout = -1;
    g_ping._options->preload = -1;
    g_ping._options->quiet = -1;
}


/**
 * @brief Initialize the time struct
*/
static void initialize_time()
{
    memset(&g_ping._time->start_time, 0, sizeof(struct timeval));
    memset(&g_ping._time->end_time, 0, sizeof(struct timeval));
    memset(&g_ping._time->send_time, 0, sizeof(struct timeval));
}


/**
 * @brief Initialize the ping struct
 */
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


/**
 * @brief Main function
 * 
 * @param ac The number of arguments
 * @param av The arguments
 */
int main(int ac, char **av)
{
    init_ping();
    parse_options(ac, av);
    dns_lookup();
    signal(SIGINT, sig_handler);
    icmp_loop();
    free_all();
    close(g_ping._sockfd);
    return 0;
}
