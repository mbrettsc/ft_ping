/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:44 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/04 16:29:41 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <ctype.h>

struct s_options {
    int count;
    int interval;
    int timeout;
    int packet_size;
    int ttl;
    int verbose;
};

struct s_ping {
    int sockfd;
    struct s_options *options;
    struct sockaddr_in dest;
    struct sockaddr_in from;
    struct iphdr *ip;
    struct icmphdr *icmp;
    char *dns;
    char *ip_address;
};

#endif