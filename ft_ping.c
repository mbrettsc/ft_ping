/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:40 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/04 17:01:03 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void free_all(struct s_ping *ping)
{
    free(ping->_options);
}

unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void dns_resolver(struct s_ping *ping)
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    
    if (getaddrinfo(ping->_dns, NULL, &hints, &res) != 0)
    {
        fprintf(stderr, "getaddrinfo: hostname cannot be resolved\n");
        free_all(ping);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    char *ip = inet_ntoa(ipv4->sin_addr);
    freeaddrinfo(res);

    ping->_ip = ip;
}

void parse_options(int ac, char **av, struct s_ping *ping)
{
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
                ping->_options->verbose = 1;
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
                ping->_options->count = atoi(av[i]);
            }
        }
        else if (av[i][0] != '-')
        {
            if (host_flag == 1)
            {
                fprintf(stderr, "Error: Too many arguments\n");
                exit(EXIT_FAILURE);
            }
            ping->_dns = av[i];
            host_flag = 1;
        }
        ++i;
    }
}

struct s_ping init_ping()
{
    struct s_ping ping;

    ping._sockfd = 0;
    ping._options = malloc(sizeof(struct s_options *));
    memset(&ping._dest, 0, sizeof(struct sockaddr_in));
    memset(&ping._icmp, 0, sizeof(struct icmphdr));
    memset(&ping._iphdr, 0, sizeof(struct iphdr));
    memset(ping._packet, 0, sizeof(char));
    ping._dns = NULL;
    ping._ip = NULL;

    return ping;
}

void send_icmp_request(struct s_ping *ping)
{
    if ((ping->_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        printf("socket fd = %d\n", ping->_sockfd);
        fprintf(stderr, "Error: An error occurred while trying to open socket, try with sudo permission!\n");
        exit(EXIT_FAILURE);
    }

    ping->_dest.sin_family = AF_INET;
    ping->_dest.sin_addr.s_addr = inet_addr(ping->_ip);
    
    ping->_icmp.type = ICMP_ECHO;
    ping->_icmp.un.echo.id = getpid();
    ping->_icmp.un.echo.sequence = 1;
    
    memcpy(ping->_packet, &ping->_icmp, sizeof(ping->_icmp));
    memset(ping->_packet + sizeof(ping->_icmp), 0, PKT_SIZE - sizeof(ping->_icmp));
    
    ping->_icmp.checksum = calculate_checksum((unsigned short *)ping->_packet, PKT_SIZE);
    memcpy(ping->_packet, &ping->_icmp, sizeof(ping->_icmp));
    
    if (sendto(ping->_sockfd, ping->_packet, PKT_SIZE, 0, (struct sockaddr *)&ping->_dest, sizeof(ping->_dest)) == -1)
    {
        perror("sendto");
        fprintf(stderr, "Error: An error occurred while trying to send ICMP packet\n");
        exit(EXIT_FAILURE);
    }
    
    printf("ICMP packet sent to %s\n", ping->_ip);

    char recv_buf[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    ssize_t bytes_received;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(ping->_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while (1)
    {
        bytes_received = recvfrom(ping->_sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (bytes_received == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("Request timed out.\n");
            }
            else
            {
                perror("recvfrom");
            }
            break;
        }

        struct iphdr *ip_hdr = (struct iphdr *)recv_buf;
        struct icmphdr *_icmp_recv = (struct icmphdr *)(recv_buf + (ip_hdr->ihl * 4));

        if (_icmp_recv->type == ICMP_ECHOREPLY && _icmp_recv->un.echo.id == getpid())
        {
            printf("ICMP packet received from %s: icmp_seq=%d\n", inet_ntoa(recv_addr.sin_addr), _icmp_recv->un.echo.sequence);
            break;
        }
    }

    close(ping->_sockfd);
}

int main(int ac, char** av)
{
    struct s_ping ping = init_ping();
    
    parse_options(ac, av, &ping);
    dns_resolver(&ping);
    send_icmp_request(&ping);
    free_all(&ping);
    
    return 0;
}
