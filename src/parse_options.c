/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_options.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 17:41:36 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/06 17:48:55 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void parse_options(int ac, char **av, struct s_ping *ping)
{
    int i = 1, host_flag = 0;

    if (ac < 2)
        print_usage();
    while (i < ac)
    {
        if (av[i][0] == '-')
        {
            // Mandatory options
            if (strcmp(av[i], "-v") == 0)
                ping->_options->verbose = 1;
            else if (strcmp(av[i], "-?") == 0)
                print_usage();
            // Mandatory options
            else if (strcmp(av[i], "-t") == 0)
            {
                check_numeric_options(av, ac, &i);
                ping->_options->ttl = atoi(av[i]);
            }
            else if (strcmp(av[i], "-f") == 0)
                ping->_options->flood = 1;   
            else if (strcmp(av[i], "-n") == 0)
                ping->_options->noreverse = 1;
            else if (strcmp(av[i], "-l") == 0)
            {
                check_numeric_options(av, ac, &i);
                ping->_options->preload = atoi(av[i]);
            }
            else if (strcmp(av[i], "-c") == 0)
            {
                check_numeric_options(av, ac, &i);
                ping->_options->count = atoi(av[i]);
                if (ping->_options->count < 1)
                {
                    fprintf(stderr, "Error: Invalid argument\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                fprintf(stderr, "Error: Invalid argument\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (av[i][0] != '-')
        {
            if (host_flag == 1)
            {
                fprintf(stderr, "Error: Too many arguments\n");
                exit(EXIT_FAILURE);
            }
            ping->_host = av[i];
            host_flag = 1;
        }
        ++i;
    }
}
