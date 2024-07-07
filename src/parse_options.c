/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_options.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 17:41:36 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 14:35:20 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static int is_whole_num(const char *str)
{
    for (int i = 0; str[i]; ++i) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

static void check_numeric_options(char **av, int ac, int *i)
{
    if (*i + 1 >= ac) {
        fprintf(stderr, "Error: Missing argument for %s\n", av[*i]);
        print_usage();
        exit(EXIT_FAILURE);
    }
    ++*i;
    if (!is_whole_num(av[*i])) {
        fprintf(stderr, "Error: Argument for %s must be an integer\n", av[*i - 1]);
        print_usage();
        exit(EXIT_FAILURE);
    }
}

static void options(char **av, int ac, int *i)
{
    if (strcmp(av[*i], "-v") == 0) {
        g_ping._options->verbose = 1;
    } else if (strcmp(av[*i], "-?") == 0) {
        print_usage();
        exit(EXIT_SUCCESS);
    } else if (strcmp(av[*i], "-t") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->ttl = atoi(av[*i]);
    } else if (strcmp(av[*i], "-f") == 0) {
        g_ping._options->flood = 1;
    } else if (strcmp(av[*i], "-n") == 0) {
        g_ping._options->noreverse = 1;
    } else if (strcmp(av[*i], "-l") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->preload = atoi(av[*i]);
    } else if (strcmp(av[*i], "-c") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->count = atoi(av[*i]);
        if (g_ping._options->count < 1) {
            fprintf(stderr, "Error: -c argument must be greater than 0\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Error: Unknown option %s\n", av[*i]);
        print_usage();
        exit(EXIT_FAILURE);
    }
}

void parse_options(int ac, char **av)
{
    int i = 1, host_flag = 0;

    if (ac < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    while (i < ac) {
        if (av[i][0] == '-') {
            options(av, ac, &i);
        } else {
            if (host_flag) {
                fprintf(stderr, "Error: Multiple hosts not allowed\n");
                exit(EXIT_FAILURE);
            }
            g_ping._host = av[i];
            host_flag = 1;
        }
        ++i;
    }

    if (!host_flag) {
        fprintf(stderr, "Error: No host specified\n");
        exit(EXIT_FAILURE);
    }
}
