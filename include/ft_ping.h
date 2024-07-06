/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:44 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/06 17:49:26 by mbrettsc         ###   ########.fr       */
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

# define PKT_SIZE 56

struct s_options {
    int count;
    int noreverse;
    int ttl;
    int verbose;
    int flood;
    int preload;
};

struct s_ping {
    int _sockfd;
    struct s_options *_options;
    char *_host;
    char *_dns;
    char *_ip;
    char *_packet[PKT_SIZE];
};

struct s_ping init_ping(void);
void parse_options(int ac, char **av, struct s_ping *ping);
void dns_resolver(struct s_ping *ping);
void free_all(struct s_ping *ping);
void dns_lookup(struct s_ping *ping);
void print_usage(void);
void check_numeric_options(char **av, int ac, int *i);

#endif