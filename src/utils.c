/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 12:18:08 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/06 17:42:24 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void free_all(struct s_ping *ping)
{
    free(ping->_options);
}

void print_usage()
{
    printf("Usage: ft_ping [-h] [-v] [-t ttl] [-f] [-n] [-l preload] [-c count] [hostname]\n");
    exit(EXIT_SUCCESS);\
}

int is_whole_num(char *str)
{
    int i = 0;

    while (str[i])
    {
        if (isdigit(str[i]) == 0)
            return 0;
        ++i;
    }
    return 1;
}

void check_numeric_options(char **av, int ac, int *i)
{
    if (*i + 1 >= ac)
    {
        fprintf(stderr, "Error: Missing argument\n");
        print_usage();
        exit(EXIT_FAILURE);
    }
    ++*i;
    if (is_whole_num(av[*i]) == 0)
    {
        fprintf(stderr, "Error: Argument must be integer\n");
        print_usage();
        exit(EXIT_FAILURE);
    }
}





struct s_ping init_ping()
{
    struct s_ping ping;

    ping._sockfd = 0;
    ping._options = malloc(sizeof(struct s_options *));
    ping._host = NULL;
    ping._ip = NULL;
    ping._dns = NULL;
    ping._options->ttl = 64;
    ping._options->count = 0;
    ping._options->verbose = 0;
    ping._options->flood = 0;
    ping._options->noreverse = 0;
    ping._options->preload = 0;

    return ping;
}

