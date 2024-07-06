/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dns_lookup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 17:42:26 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/06 17:42:33 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void reverse_dns_lookup(struct s_ping *ping) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char *hostname = malloc(NI_MAXHOST * sizeof(char));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ping->_ip);

    if (getnameinfo((struct sockaddr *)&addr, len, hostname, NI_MAXHOST, NULL, 0, 0) != 0) {
        free(hostname);
        exit(EXIT_FAILURE);
    }
    
    ping->_dns = hostname;
}

void dns_lookup(struct s_ping *ping)
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    
    if (getaddrinfo(ping->_host, NULL, &hints, &res) != 0)
    {
        fprintf(stderr, "ft_ping: %s: Name or service not known\n", ping->_host);
        free_all(ping);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    char *ip = inet_ntoa(ipv4->sin_addr);
    freeaddrinfo(res);

    ping->_ip = ip;
    reverse_dns_lookup(ping);
}