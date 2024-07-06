/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:31:40 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/06 17:48:46 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

struct ping_pkt {
    struct icmphdr hdr;
    char msg[PKT_SIZE];
};

void exit_error(char *err_msg)
{
    fprintf(stderr, "ft_ping: error: %s", err_msg);
    exit(EXIT_FAILURE);
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

void current_time_microseconds(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

// Calculate the difference in milliseconds between two timevals
double time_difference(struct timeval *start, struct timeval *end)
{
    return (double)((end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0);
}

// Main ping loop function
void icmp_loop(struct s_ping *ping) {
    struct sockaddr_in dest_addr, recv_addr;
    struct ping_pkt pckt;
    int sequence_number = 1, packets_sent = 0, packets_received = 0;
    struct timeval send_time, receive_time;
    char recv_buffer[84];

    
    if ((ping->_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
        exit_error("failed to open socket fd");

    if (setsockopt(ping->_sockfd, IPPROTO_IP, IP_TTL, &ping->_options->ttl, sizeof(ping->_options->ttl)) != 0)
        exit_error("failet to set socket options");

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(ping->_ip);

    socklen_t addr_len = sizeof(dest_addr);

    printf("PING %s (%s) 56(84) bytes of data.\n", ping->_host, ping->_ip);

    while (1)
    {
        memset(&pckt, 0, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = getpid();
        pckt.hdr.un.echo.sequence = sequence_number++;
        pckt.hdr.checksum = calculate_checksum(&pckt, sizeof(pckt));

        current_time_microseconds(&send_time);

        if (sendto(ping->_sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0)
            exit_error("failed to send packet");

        if (recvfrom(ping->_sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&recv_addr, &addr_len) <= 0)
            exit_error("failed to receive packet");
        else
        {
            current_time_microseconds(&receive_time);
            double rtt = time_difference(&send_time, &receive_time);

            // Extract TTL from the IP header
            struct iphdr *ip_hdr = (struct iphdr *)recv_buffer;
            int ttl = ip_hdr->ttl;
            
            // Extract ICMP header and sequence number
            struct icmphdr *icmp_hdr = (struct icmphdr *)(recv_buffer + (ip_hdr->ihl * 4));
            int sequence = icmp_hdr->un.echo.sequence;
            
            printf("64 bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n", 
                   ping->_dns, ping->_ip, sequence, ttl, rtt);

            packets_received++;
        }

        if (ping->_options->count != 0 && sequence_number >= ping->_options->count)
            break;
        
        packets_sent++;
        sleep(1);
    }
    printf("\n--- %s ping statistics ---\n", ping->_ip);
    printf("%d packets transmitted, %d received, %.1f%% packet loss\n",
           packets_sent, packets_received, ((packets_sent - packets_received) / (float)packets_sent) * 100.0);
}


int main(int ac, char** av)
{
    struct s_ping ping = init_ping();
    
    parse_options(ac, av, &ping);
    dns_lookup(&ping);
    icmp_loop(&ping);
    free_all(&ping);
    
    return 0;
}
