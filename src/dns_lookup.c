/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dns_lookup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 17:42:26 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 14:41:25 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void reverse_dns_lookup(const char *ip_address)
{
    struct sockaddr_in addr;
    char hostname[NI_MAXHOST];

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_address, &addr.sin_addr) <= 0) {
        exit_error("Invalid IP address format");
    }

    if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), hostname, NI_MAXHOST, NULL, 0, NI_NAMEREQD) != 0) {
        exit_error("Failed to perform reverse DNS lookup");
    }

    g_ping._dns = strdup(hostname);
    if (!g_ping._dns) {
        exit_error("Failed to allocate memory for DNS name");
    }
}

void dns_lookup(void)
{
    struct addrinfo hints, *res;
    char ip_str[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_RAW; // Raw socket

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

    reverse_dns_lookup(g_ping._ip);
}
