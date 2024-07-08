/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_options.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 17:41:36 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/08 13:40:55 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"


/**
 * @brief Check if a string is a whole number
 * 
 * @param str The string to check
 */
static inline int is_whole_num(const char *str)
{
    for (int i = 0; str[i]; ++i) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}


/**
 * @brief Check if the options that require a numeric argument are valid
 * 
 * @param av The arguments
 * @param ac The number of arguments
 * @param i The index of the current argument
 */
static inline void check_numeric_options(char **av, int ac, int *i)
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


/**
 * @brief Parse the command line options
 * 
 * @param ac The number of arguments
 * @param av The arguments
 */
static inline void options(char **av, int ac, int *i)
{
    if (strcmp(av[*i], "-v") == 0) {
        g_ping._options->verbose = 1;
    }
    else if (strcmp(av[*i], "-?") == 0) {
        print_usage();
        exit(EXIT_SUCCESS);
    }
    else if (strncmp(av[*i], "--ttl=", 6) == 0) {
        char *tmp = av[*i] + 6;
        if (is_whole_num(tmp) == 0)
            exit_error("ttl value must be a positive integer");
        int ttl_value = atoi(tmp);
        if (ttl_value <= 0 || ttl_value >= 256) {
            exit_error("option value too small or too big");
        }
        g_ping._options->ttl = ttl_value;
    }
    else if (strcmp(av[*i], "-w") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->timeout = atoi(av[*i]);
        if (g_ping._options->timeout < 1) {
            exit_error("timeout value must be a positive integer");
        }
    }
    else if (strcmp(av[*i], "-l") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->preload = atoi(av[*i]);
    }
    else if (strcmp(av[*i], "-c") == 0) {
        check_numeric_options(av, ac, i);
        g_ping._options->count = atoi(av[*i]);
        if (g_ping._options->count < 1) {
            exit_error("count value must be a positive integer");
        }
        
    }
    else if (strcmp(av[*i], "-q") == 0) {
        g_ping._options->quiet = 1;
    }
    else {
        print_usage();
        exit_error("unkown option");
    }
}


/**
 * @brief Parse the command line options
 * 
 * @param ac The number of arguments
 * @param av The arguments
 */
void parse_options(int ac, char **av)
{
    int i = 1, host_flag = 0;

    if (ac < 2) {
        exit_error("missing host operand\nTry 'ping --help' or 'ping --usage' for more information.");
    }
    
    while (i < ac) {
        if (av[i][0] == '-') {
            options(av, ac, &i);
        } else {
            if (host_flag) {
                exit_error("multiple host not allowed");
            }
            g_ping._host = av[i];
            host_flag = 1;
        }
        ++i;
    }

    if (!host_flag) {
        exit_error("No host specified");
    }

    if (geteuid() != 0) {
        exit_error("Lacking privilege for icmp socket.");
    }
}
