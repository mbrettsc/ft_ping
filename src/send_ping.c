/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrettsc <mbrettsc@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 12:26:48 by mbrettsc          #+#    #+#             */
/*   Updated: 2024/07/08 14:01:18 by mbrettsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"


/**
 * @brief Returns the difference between two timeval structs in milliseconds.
 *
 *
 * @param start The start time.
 * @param end The end time.
 * @return double The difference between the two times in milliseconds.
 */
static inline double time_difference(struct timeval *start, struct timeval *end)
{
    return (double)((end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0);
}


/**
 * @brief Calculates the checksum of a buffer.
 *
 * @param b The buffer to calculate the checksum of.
 * @param len The length of the buffer.
 * 
 * @return unsigned short The checksum of the buffer.
 */
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


/**
 * @brief Prints the statistics of the ping session.
 * 
 * This function prints the statistics of the ping session, such as the number of packets sent and received,
 * the packet loss percentage, the total time of the session, the minimum, average, maximum, and mean deviation
 * of the round-trip time.
 */
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
    printf("--- %s ping statistics ---\n", g_ping._host);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
           packets_sent, 
           packets_received,
           (int) packet_lost);
    if (g_ping._rtt->count > 0) {
        printf("rount-trip min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
            g_ping._rtt->min_rtt, avg_rtt, g_ping._rtt->max_rtt, mdev_rtt);
    }
}


/**
 * @brief Sets up the socket for the ICMP connection.
 * 
 * This function sets up the socket for the ICMP connection, and sets the TTL option for the socket.
 * If the verbose option is set, it also sets the multicast loop option for the socket.
 */
static void setup_socket(void)
{
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


/**
 * @brief Sends a ping packet to the destination.
 * 
 * This function sends a ping packet to the destination. It sets up the destination address, the ICMP header,
 * and the checksum of the packet. It then sends the packet to the destination address.
 * 
 * @param sequence_number The sequence number of the packet.
 */
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


/**
 * @brief Prints the information of a received packet.
 * 
 * This function prints the information of a received packet, such as the source and destination IP addresses,
 * the ICMP type, the ICMP sequence number, the time to live, and the round-trip time.
 * 
 * @param ip_hdr The IP header of the received packet.
 * @param icmp_hdr The ICMP header of the received packet.
 * @param rtt_ms The round-trip time of the packet.
 * @param icmp_sequence The ICMP sequence number of the packet.
 */
static inline void print_packet_info(struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, double rtt_ms, int icmp_sequence)
{
    int ip_header_length = ip_hdr->ihl * 4;

    if (strcmp(g_ping._ip, "127.0.0.1") == 0 || strcmp(g_ping._ip, "localhost") == 0) {
        icmp_sequence = g_ping._packets_sent - 1;
    }
    
    if (g_ping._options->verbose == 1) {
        if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
            struct iphdr *original_ip_hdr = (struct iphdr *)((char *)ip_hdr + ip_header_length + sizeof(struct icmphdr));
            struct in_addr original_src_ip, original_dst_ip;

            original_src_ip.s_addr = original_ip_hdr->saddr;
            original_dst_ip.s_addr = original_ip_hdr->daddr;

            printf("IP Hdr Dump:\n");
            printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks    Src            Dst\n");
            printf(" %1x  %1x  %02x  %04x %04x  %02x  %04x %02x  %04x  %d  %s   %s\n",
                original_ip_hdr->version,
                original_ip_hdr->ihl,
                original_ip_hdr->tos,
                ntohs(original_ip_hdr->tot_len),
                ntohs(original_ip_hdr->id),
                (ntohs(original_ip_hdr->frag_off) & 0xE000) >> 13,
                ntohs(original_ip_hdr->frag_off) & 0x1FFF,
                original_ip_hdr->ttl,
                original_ip_hdr->protocol,
                ntohs(original_ip_hdr->check),
                inet_ntoa(original_src_ip),
                inet_ntoa(original_dst_ip));
        } else if (icmp_hdr->type == ICMP_ECHOREPLY || icmp_hdr->type == ICMP_ECHO) {
            printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                ntohs(ip_hdr->tot_len) - (ip_hdr->ihl * 4) - sizeof(struct icmphdr) - 4,
                g_ping._ip,
                icmp_sequence,
                ip_hdr->ttl,
                rtt_ms);
        } else {
            fprintf(stderr, "Unknown ICMP type: %d in verbose mode.\n", icmp_hdr->type);
        }
    } else {
        if (icmp_hdr->type == ICMP_ECHOREPLY || icmp_hdr->type == ICMP_ECHO) {
            printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                ntohs(ip_hdr->tot_len) - (ip_hdr->ihl * 4) - sizeof(struct icmphdr) - 4,
                g_ping._ip,
                icmp_sequence,
                ip_hdr->ttl,
                rtt_ms);
        } else if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
            struct iphdr *original_ip_hdr = (struct iphdr *)((char *)ip_hdr + ip_header_length + sizeof(struct icmphdr));
            struct in_addr original_src_ip;

            original_src_ip.s_addr = original_ip_hdr->saddr;
            printf("%d bytes from (%s): Time to live exceeded\n",
                ntohs(ip_hdr->tot_len) - ip_header_length,
                inet_ntoa(original_src_ip));
        } else {
            fprintf(stderr, "Unknown ICMP type: %d\n", icmp_hdr->type);
        }
    }
}


/**
 * @brief Updates the round-trip time statistics.
 * 
 * This function updates the round-trip time statistics, such as the minimum, maximum, and average round-trip time,
 * the total round-trip time, and the total round-trip time squared.
 * 
 * @param rtt_ms The round-trip time of the packet.
 */
static void update_rtt_stats(double rtt_ms)
{
    g_ping._rtt->total_rtt += rtt_ms;
    if (rtt_ms < g_ping._rtt->min_rtt || g_ping._rtt->min_rtt == 0) {
        g_ping._rtt->min_rtt = rtt_ms;
    }
    if (rtt_ms > g_ping._rtt->max_rtt) {
        g_ping._rtt->max_rtt = rtt_ms;
    }
    g_ping._rtt->total_rtt_squared += rtt_ms * rtt_ms;
    g_ping._rtt->count++;
}


/**
 * @brief Handles a received packet.
 * 
 * This function handles a received packet. It calculates the round-trip time of the packet, updates the round-trip
 * time statistics, and prints the information of the packet.
 * 
 * @param recv_buffer The buffer containing the received packet.
 */
static inline void handle_received_packet(char *recv_buffer)
{
    struct timeval receive_time;
    gettimeofday(&receive_time, NULL);

    double rtt_ms = time_difference(&g_ping._time->send_time, &receive_time);
    update_rtt_stats(rtt_ms);
    

    struct iphdr *ip_hdr = (struct iphdr *)recv_buffer;
    struct icmphdr *icmp_hdr = (struct icmphdr *)(recv_buffer + (ip_hdr->ihl * 4));
    int icmp_sequence = icmp_hdr->un.echo.sequence;
    
    if (g_ping._options->quiet == 1) return;

    print_packet_info(ip_hdr, icmp_hdr, rtt_ms, icmp_sequence);
}


/**
 * @brief Receives a ping packet.
 * 
 * This function receives a ping packet. It sets up the file descriptor set for the socket, and waits for a packet
 * to be received. If a packet is received, it reads the packet into a buffer, and handles the received packet.
 * 
 */
static void receive_ping()
{
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
            g_ping._packets_received++;
            handle_received_packet(recv_buffer);
        }
    }
}


/**
 * @brief Checks if the timeout has been reached.
 * 
 * This function checks if the timeout has been reached. It calculates the elapsed time since the start of the session,
 * and compares it to the timeout value. If the elapsed time is greater than the timeout value, the function returns 0,
 * otherwise it returns 1.
 * 
 * @return int 0 if the timeout has been reached, 1 otherwise.
 */
int check_timeout(void)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    double elapsed_time = time_difference(&g_ping._time->start_time, &current_time);
    double elapsed_seconds = floor(elapsed_time / 1000.0);
    
    if (elapsed_seconds > g_ping._options->timeout) {
        return 0;
    }
    return 1;
}


/**
 * @brief The main ICMP loop.
 * 
 * This function is the main ICMP loop. It sets up the socket, prints the initial information of the session,
 * and enters a loop where it sends and receives ping packets. It also checks for the timeout value, and breaks
 * the loop if the timeout has been reached. After the loop, it prints the statistics of the session.
 */
void icmp_loop(void)
{
    setup_socket();
    if (g_ping._options->verbose == 1) {
        printf("PING %s (%s): %d data bytes, id 0x%x = %d\n", g_ping._host, g_ping._ip, PKT_SIZE, getpid(), getpid());
    } else {
        printf("PING %s (%s): %d data bytes\n", g_ping._host, g_ping._ip, PKT_SIZE);
    }
    gettimeofday(&g_ping._time->start_time, NULL);
    for (int sequence_number = 0; g_ping._options->count == -1 || sequence_number < g_ping._options->count; ++sequence_number) {
        send_ping(sequence_number);
        receive_ping();

        if (sequence_number == g_ping._options->count - 1)
            break;
        if (g_ping._options->preload == -1) {
            sleep(1);
        } else if (sequence_number >= g_ping._options->preload) {
            g_ping._options->preload = 0;
            sleep(1);
        }
        if (g_ping._options->timeout != -1) {
            if (check_timeout() == 0) {
                break;
            }
        }
    }
    print_statistics();
}
