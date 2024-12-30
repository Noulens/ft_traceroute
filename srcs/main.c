//
// Created by tnoulens on 4/14/24.
//

#include "../include/traceroute.h"

int	g_traceroute_flag = GO;

/*void	tmp_handler(int useless)
{
	(void)useless;
	g_ping_flag &= ~GO;
}*/

/*int main(int ac, char **av)
{
	struct icmphdr      *r_icmp_hdr = NULL;
	struct icmp_filter  filter;
	struct iphdr        *ipHdr = NULL;
	struct sockaddr_in  target, r_addr;
	struct timeval      timeout;
	socklen_t           r_addr_len;
	ssize_t             must_do;
	double              avg = 0, min = THE_MAX, max = 0;
	t_ppckt             icmp_hdr;
	int                 socket_fd = -1, nb_packets = 0, nb_r_packets = 0;
	int                 count, interval = PING_USLEEP_RATE, linger = RECV_TIMEOUT;
	char                ttl_val = 64;
	char                packet[1024];
	char                buffer[ADDR_LEN];
	char                error_buffer[255];
	char                from[NI_MAXHOST];

	signal(SIGINT, tmp_handler);
	ft_bzero(&target, sizeof(target));
	ft_bzero(buffer, ADDR_LEN);
	check_args(ac, av, &count, &ttl_val, &linger, &interval, buffer);
	socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_fd == -1)
		error("socket() failed", errno, TRUE);
	if (is_valid_ip(buffer, &target) != 1)
	{
		if (close(socket_fd) == -1)
			error("close()", errno, TRUE);
		error("unknown host", -1, TRUE);
	}
	timeout.tv_sec = linger;
	timeout.tv_usec = 0;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == -1)
		return (close(socket_fd), ft_fprintf(2, "setsockopt() failed: %s", strerror(errno)), EXIT_FAILURE);
	icmp_filter_set_block_all(&filter);
	icmp_filter_set_pass(ICMP_ECHOREPLY, &filter);
	icmp_filter_set_pass(ICMP_TIME_EXCEEDED, &filter);
	icmp_filter_set_pass(ICMP_DEST_UNREACH, &filter);
	icmp_filter_set_pass(ICMP_REDIRECT, &filter);
	if (setsockopt(socket_fd, SOL_RAW, ICMP_FILTER, &filter, sizeof(filter)) == -1)
		return (close(socket_fd), ft_fprintf(2, "setsockopt() failed: %s", strerror(errno)), EXIT_FAILURE);
	if (setsockopt(socket_fd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
		return (close(socket_fd), ft_fprintf(2, "setsockopt() failed: %s", strerror(errno)), EXIT_FAILURE);
	while (count == -1 ? (g_ping_flag & GO) : (g_ping_flag & GO) && count--)
	{
		must_do = 0;
		prepare_packet(&icmp_hdr, &nb_packets);
		must_do = sendto(socket_fd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&target, sizeof(target));
		if (must_do <= 0)
			error("sending packet", errno, TRUE);
		double  start_count = gettimeinms();
		r_addr_len = sizeof(r_addr);
		struct iovec iov;
		struct msghdr msg;
		if (must_do > 0)
		{
			ft_bzero(packet, sizeof(packet));
			iov.iov_base = packet;
			iov.iov_len = sizeof(packet);
			msg.msg_name = (void *)&r_addr;
			msg.msg_namelen = r_addr_len;
			msg.msg_iov = &iov;
			msg.msg_iovlen = 1;
			msg.msg_flags = 0;
			msg.msg_control = NULL;
			msg.msg_controllen = 0;
			must_do = recvmsg(socket_fd, &msg, 0);
			if (must_do > 0)
			{
				ipHdr = (struct iphdr *)packet;
				r_icmp_hdr = (struct icmphdr *) (packet + sizeof(struct iphdr));
//				char *r_buffer;
//				r_buffer = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));
//				print_reply(r_icmp_hdr, r_buffer);
				analyze_packet(r_icmp_hdr, &nb_r_packets, error_buffer);
			}
		}
		double  end_count = (gettimeinms() - start_count);
		if (must_do > 0)
		{
			if (end_count < min)
				min = end_count;
			if (end_count > max)
				max = end_count;
			avg += end_count;
			if (!(r_icmp_hdr->type == ICMP_ECHOREPLY && r_icmp_hdr->code == 0))
			{
				getnameinfo((struct sockaddr *) &r_addr, r_addr_len, from, NI_MAXHOST, NULL, 0, 0);
				if (!(g_ping_flag & VERBOSE))
					printf("%zu bytes from %s (%s): %s\n", must_do - sizeof(struct iphdr), from,
						   inet_ntoa(r_addr.sin_addr), error_buffer);
				else
					printf("%zu bytes from %s (%s): type: %d, code: %d: %s\n", must_do - sizeof(struct iphdr),
						   from, inet_ntoa(r_addr.sin_addr), r_icmp_hdr->type, r_icmp_hdr->code, error_buffer);
			}
			else if (!(g_ping_flag & QUIET))
				printf("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", must_do - sizeof(struct iphdr),
					   inet_ntoa(r_addr.sin_addr), ntohs(r_icmp_hdr->un.echo.sequence), ipHdr->ttl, (double)end_count);
		}
		usleep(interval * M);
	}
	printf("--- %s ping statistics ---\n", buffer);
	printf("%d packets transmitted, %d packets received, %0.1f%% packet loss\n", nb_packets, nb_r_packets, \
		(((float)nb_packets - (float)nb_r_packets) * 100.0) / (float)nb_packets);
	if (nb_r_packets)
		printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n", min == THE_MAX ? 0 : (double)min, (double)avg / nb_packets, (double)max);
	else
		ft_putchar_fd('\n', 1);
	if (close(socket_fd) == -1)
		return (fprintf(stderr, "close() failed: %s", strerror(errno)), EXIT_FAILURE);
	return (0);
}*/

void print_traceroute(char buffer[1024], t_traceroute traceroute)
{
	printf("target: %s\n", buffer);
	printf("first: %d\n", traceroute.first_ttl);
	printf("max: %d\n", traceroute.max_ttl);
	printf("sendwait: %.02f\n", traceroute.send_wait);
	printf("queries: %d\n", traceroute.n_queries);
	printf("port: %d\n", traceroute.port);
}

void create_sockets(t_traceroute *traceroute)
{
	if ((traceroute->flags & ICMP_PROBE && (traceroute->fd[SO_SND] = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) <= -1)
		|| ((traceroute->flags & ICMP_PROBE) == 0 && (traceroute->fd[SO_SND] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= -1))
		error("create send socket", errno, TRUE);
	if (traceroute->flags & ICMP_PROBE)
		traceroute->fd[SO_RECV] = traceroute->fd[SO_SND];
	else if ((traceroute->fd[SO_RECV] = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) <= -1)
	{
		close(traceroute->fd[SO_SND]);
		error("create recv socket", errno, TRUE);
	}
}

void close_all(t_traceroute *traceroute)
{
	for (int i = 0; i < 2; i++)
	{
		if (traceroute->fd[i] > 0)
		{
			close(traceroute->fd[i]);
			traceroute->fd[i] = -1;
		}
	}
}

int main(int const ac, char **av)
{
	char				buffer[ADDR_LEN];
	t_traceroute		traceroute;
	struct sockaddr_in	dest_addr, recv_addr;
	socklen_t			recv_len = sizeof(recv_addr);
	//int					nb_packets = 0;
	//t_icmp_packet		icmp_hdr;

	ft_memset(buffer, 0, ADDR_LEN);
	traceroute = check_args(ac, av, buffer);
	create_sockets(&traceroute);

	// TODO: DNS & Destination address
	ft_memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_port = htons(traceroute.port);
	if (is_valid_ip(buffer, &dest_addr) != 1)
	{
		close_all(&traceroute);
		error("unknown host", -1, TRUE);
	}

	// Main loop
	for (int ttl = traceroute.first_ttl; ttl <= traceroute.max_ttl; ttl++)
	{
		if (setsockopt(traceroute.fd[SO_SND], IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
		{
			error("setsockopt ttl", errno, FALSE);
			break;
		}
		// icmp_hdr = prepare_packet(&nb_packets);
		// sendto(traceroute.fd[SO_SND], &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0
		if (sendto(traceroute.fd[SO_SND], NULL, 0, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
		{
			error("sento", errno, FALSE);
			break;
		}
		struct timeval timeout = {3, 0};
		if (setsockopt(traceroute.fd[SO_RECV], SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
		{
			error("setsockopt timeout", errno, FALSE);
			break;
		}
		char bufferres[512];
		if (recvfrom(traceroute.fd[SO_RECV], bufferres, sizeof(bufferres), 0, (struct sockaddr *)&recv_addr, &recv_len) < 0)
		{
			printf("%2d  *  Timeout: %s\n", ttl, strerror(errno));
			continue;
		}

		const struct icmphdr	*r_icmp_hdr = NULL;
		r_icmp_hdr = (struct icmphdr *) (bufferres + sizeof(struct iphdr));
		const char *r_buffer = bufferres + sizeof(struct iphdr) + sizeof(struct icmphdr);
		print_reply(r_icmp_hdr, r_buffer);

		if (r_icmp_hdr->type == ICMP_TIME_EXCEEDED)
		{
			printf("%2d  %s\n", ttl, inet_ntoa(recv_addr.sin_addr));
		}
		else if (r_icmp_hdr->type == ICMP_DEST_UNREACH)
		{
			printf("%2d  %s (Reached destination)\n", ttl, inet_ntoa(recv_addr.sin_addr));
			break;
		}
	}
	close_all(&traceroute);
	return 0;
}
