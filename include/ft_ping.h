/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:44 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 14:15:57 by mbrettsc         ###   ########.fr       */
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

struct ping_pkt {
    struct icmphdr hdr;
    unsigned short id;
    char msg[PKT_SIZE];
};

struct s_time {
    struct timeval start_time;
    struct timeval end_time;
    struct timeval send_time;
};

struct s_options {
    int count;
    int noreverse;
    int ttl;
    int verbose;
    int flood;
    int preload;
};

struct s_rtt {
    double min_rtt;
    double max_rtt;
    double total_rtt;
    double total_rtt_squared;
    double total_time_ms;
    int count;
};

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
void dns_lookup(void);
void icmp_loop(void);
void print_statistics(void);

#endif