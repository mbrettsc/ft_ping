/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 12:26:48 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/07 14:33:08 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void print_socket_info(void) {
    printf("ping: sock4.fd: %d (socktype: SOCK_RAW)(socktype: SOCK_RAW), hints.ai_family: AF_UNSPEC\n", g_ping._sockfd);
    printf("\nai->ai_family: AF_INET");
    printf(", ai->ai_canonname: '%s'\n", g_ping._host);
}

static inline double time_difference(struct timeval *start, struct timeval *end)
{
    return (double)((end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0);
}

static inline unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void print_statistics(void)
{
    gettimeofday(&g_ping._time->end_time, NULL);
    g_ping._rtt->total_time_ms = time_difference(&g_ping._time->start_time, &g_ping._time->end_time);
    double avg_rtt = g_ping._rtt->total_rtt / g_ping._rtt->count;
    double variance = (g_ping._rtt->total_rtt_squared / g_ping._rtt->count) - (avg_rtt * avg_rtt);
    double mdev_rtt = sqrt(variance);

    int packets_sent = g_ping._packets_sent, packets_received = g_ping._packets_received;
    double packet_lost = ((packets_sent - packets_received) / (float)packets_sent);
    if (packet_lost < 0 || (packets_sent == 0 && packets_received == 0)) {
        packet_lost = 1;
    }
    packet_lost *= 100.0;
    if (packets_sent <= 0)  {
        packets_sent = 0;
    }
    if (packets_received <= 0) {
        packets_received = 0;
    }
    printf("\n--- %s ping statistics ---\n", g_ping._host);
    printf("%d packets transmitted, %d received, %.1f%% packet loss, time %.0fms\n",
           packets_sent, 
           packets_received,
           packet_lost,
           g_ping._rtt->total_time_ms);
    if (g_ping._rtt->count > 0) {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
            g_ping._rtt->min_rtt, avg_rtt, g_ping._rtt->max_rtt, mdev_rtt);
    }
}

static void setup_socket(void) {
    if ((g_ping._sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        exit_error("failed to open socket fd");
    }

    if (setsockopt(g_ping._sockfd, IPPROTO_IP, IP_TTL, &g_ping._options->ttl, sizeof(g_ping._options->ttl)) != 0) {
        exit_error("failed to set socket options");
    }

    if (g_ping._options->verbose == 1) {
        int sockopt = 1;
        if (setsockopt(g_ping._sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &sockopt, sizeof(sockopt)) < 0) {
            exit_error("failed to set multicast loop");
        }
    }
}

static void send_ping(int sequence_number)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;
    dest_addr.sin_addr.s_addr = inet_addr(g_ping._ip);

    struct ping_pkt pckt;
    memset(&pckt, 0, sizeof(pckt));
    pckt.hdr.type = ICMP_ECHO;
    pckt.hdr.un.echo.id = getpid();
    pckt.hdr.un.echo.sequence = sequence_number;
    pckt.hdr.checksum = calculate_checksum(&pckt, sizeof(pckt));

    gettimeofday(&g_ping._time->send_time, NULL);

    if (sendto(g_ping._sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
        exit_error("failed to send packet");
    }

    g_ping._packets_sent++;
}

static void handle_received_packet(char *recv_buffer) {
    struct timeval receive_time;
    gettimeofday(&receive_time, NULL);

    double rtt_ms = time_difference(&g_ping._time->send_time, &receive_time);

    g_ping._rtt->total_rtt += rtt_ms;
    if (rtt_ms < g_ping._rtt->min_rtt || g_ping._rtt->min_rtt == 0) {
        g_ping._rtt->min_rtt = rtt_ms;
    }
    if (rtt_ms > g_ping._rtt->max_rtt) {
        g_ping._rtt->max_rtt = rtt_ms;
    }
    g_ping._rtt->total_rtt_squared += rtt_ms * rtt_ms;
    g_ping._rtt->count++;

    struct iphdr *ip_hdr = (struct iphdr *)recv_buffer;
    int ttl = ip_hdr->ttl;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(recv_buffer + (ip_hdr->ihl * 4));
    int icmp_sequence = icmp_hdr->un.echo.sequence;
    unsigned short id = ((struct ping_pkt *)recv_buffer)->id;

    if (g_ping._options->verbose == 1) {
        if (g_ping._options->noreverse != 1) {
            printf("64 bytes from %s (%s): icmp_seq=%d ident=%hu ttl=%d time=%.1f ms\n",
                   g_ping._dns, g_ping._ip, icmp_sequence, id, ttl, rtt_ms);
        } else {
            printf("64 bytes from %s: icmp_seq=%d ident=%hu ttl=%d time=%.1f ms\n",
                   g_ping._ip, icmp_sequence, id, ttl, rtt_ms);
        }  
    } else {
        if (g_ping._options->noreverse != 1) {
            printf("64 bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
                   g_ping._dns, g_ping._ip, icmp_sequence, ttl, rtt_ms);
        } else {
            printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
                g_ping._ip, icmp_sequence, ttl, rtt_ms);
        }
    }
    g_ping._packets_received++;
}

static void receive_ping() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(g_ping._sockfd, &readfds);

    struct timeval timeout = {1, 0};
    int select_ret = select(g_ping._sockfd + 1, &readfds, NULL, NULL, &timeout);
    if (select_ret < 0) {
        exit_error("select failed");
    }

    if (FD_ISSET(g_ping._sockfd, &readfds)) {
        char recv_buffer[84];
        struct sockaddr_in recv_addr;
        socklen_t addr_len = sizeof(recv_addr);

        int bytes_received = recvfrom(g_ping._sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
        if (bytes_received > 0) {
            handle_received_packet(recv_buffer);
        }
    }
}

void icmp_loop(void)
{
    setup_socket();
    if (g_ping._options->verbose == 1) {
        print_socket_info();
    }

    printf("PING %s (%s) 56(84) bytes of data.\n", g_ping._host, g_ping._ip);
    gettimeofday(&g_ping._time->start_time, NULL);
    for (int sequence_number = 1; g_ping._options->count == 0 || sequence_number <= g_ping._options->count; ++sequence_number) {
        send_ping(sequence_number);
        receive_ping(sequence_number);

        if (sequence_number == g_ping._options->count)
            break;
        if (g_ping._options->preload == 0) {
            sleep(1);
        } else if (sequence_number >= g_ping._options->preload) {
            g_ping._options->preload = 0;
            sleep(1);
        }
    }
    print_statistics();
}
