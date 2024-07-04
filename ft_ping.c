/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:40 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/04 15:02:23 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <getopt.h>

char* dns_resolver(const char *hostname)
{
    struct addrinfo hints, *res;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    
    if (getaddrinfo(hostname, NULL, &hints, &res) != 0)
    {
        fprintf(stderr, "getaddrinfo: hostname cannot be resolved\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    char *ip_address = inet_ntoa(ipv4->sin_addr);
    freeaddrinfo(res);

    return ip_address;
}

void parse_options(int ac, char **av, struct s_ping *ping)
{
    char *hostname;
    int i = 1, host_flag = 0;

    if (ac < 2)
    {
        printf("Usage: %s [hostname]\n", av[0]);
        exit(EXIT_SUCCESS);
    }
    printf("Number of arguments: %d\n", ac);
    while (i < ac)
    {
        if (av[i][0] == '-')
        {
            if (strcmp(av[i], "-v") == 0)
            {
                ping->options->verbose = 1;
            }
            else if (strcmp(av[i], "-h") == 0)
            {
                printf("Usage: %s [hostname]\n", av[0]);
                exit(EXIT_SUCCESS);
            }
            else if (strcmp(av[i], "-c") == 0)
            {
                if (i + 1 >= ac)
                {
                    fprintf(stderr, "Error: Missing argument: Count must be an integer number\n");
                    exit(EXIT_FAILURE);
                }
                ++i;
                int j = 0;
                while (av[i][j])
                {
                    if (isdigit(av[i][j]) == 0)
                    {
                        fprintf(stderr, "Error: Invalid argument: Count must be an integer number\n");
                        exit(EXIT_FAILURE);
                    }
                    ++j;
                }
                ping->options->count = atoi(av[i]);
            }
        }
        else if (av[i][0] != '-')
        {
            printf("debug 2\n");
            if (host_flag == 1)
            {
                fprintf(stderr, "Error: Too many arguments\n");
                exit(EXIT_FAILURE);
            }
            hostname = av[i];
            host_flag = 1;
        }
        ++i;
    }
    printf("Count: %d\n", ping->options->count);
    
    printf("Hostname: %s\n", hostname);
}

struct s_ping init_ping()
{
    struct s_ping ping;

    ping.ip = NULL;
    ping.icmp = NULL;
    ping.dns = NULL;
    ping.options = malloc(sizeof(struct s_options));    

    return ping;
}

void free_all(struct s_ping *ping)
{
    free(ping->options);
}

int main(int ac, char** av)
{
    struct s_ping ping = init_ping();
    parse_options(ac, av, &ping);
    char * ip_address = dns_resolver(av[1]);
    printf("IP address: %s\n", ip_address);
    free_all(&ping);
    return 0;
}
