/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:44 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/08 13:27:49 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <string.h>
# include <time.h>
# include <errno.h>
# include <ctype.h>
# include <signal.h>
# include <math.h>
# include <float.h>
# include <sys/select.h>

# define PKT_SIZE 56

/**
 * @brief Structure to hold the ICMP packet
 * 
 * @param hdr The ICMP header
 * @param id The ICMP packet ID
 * @param msg The ICMP packet message
 */
struct ping_pkt {
    struct icmphdr hdr;
    unsigned short id;
    char msg[PKT_SIZE];
};


/**
 * @brief Structure to hold the time values
 * 
 * @param start_time The start time of the ping
 * @param end_time The end time of the ping
 * @param send_time The time the packet was sent
 */
struct s_time {
    struct timeval start_time;
    struct timeval end_time;
    struct timeval send_time;
};

/**
 * @brief Structure to hold the options
 * 
 * @param count The number of packets to send
 * @param ttl The time to live
 * @param verbose The verbose flag
 * @param quiet The quiet flag
 * @param timeout The timeout value
 * @param preload The number of packets to send at once
 */
struct s_options {
    int count;
    int ttl;
    int verbose;
    int quiet;
    int timeout;
    int preload;
};

/**
 * @brief Structure to hold the round trip time values
 * 
 * @param min_rtt The minimum round trip time
 * @param max_rtt The maximum round trip time
 * @param total_rtt The total round trip time
 * @param total_rtt_squared The total round trip time squared
 * @param total_time_ms The total time in milliseconds
 * @param count The number of packets sent
 */
struct s_rtt {
    double min_rtt;
    double max_rtt;
    double total_rtt;
    double total_rtt_squared;
    double total_time_ms;
    int count;
};


/**
 * @brief Structure to hold the ping values
 * 
 * @param _sockfd The socket file descriptor
 * @param _packets_sent The number of packets sent
 * @param _packets_received The number of packets received
 * @param _host The host name
 * @param _dns The DNS name
 * @param _ip The IP address
 * @param _rtt The round trip time values
 * @param _time The time values
 * @param _options The options
 */
struct s_ping {
    int _sockfd;
    int _packets_sent;
    int _packets_received;
    char *_host;
    char *_dns;
    char *_ip;
    struct s_rtt *_rtt;
    struct s_time *_time;
    struct s_options *_options;
};

extern struct s_ping g_ping;

void parse_options(int ac, char **av);
void exit_error(char *err_msg);
void free_all(void);
void print_usage(void);
void icmp_loop(void);
void print_statistics(void);

#endif