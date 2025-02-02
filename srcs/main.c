//
// Created by tnoulens on 4/14/24.
//

#include "../include/traceroute.h"

void close_all(t_traceroute *traceroute)
{
	int	er = 0;

	if (traceroute->flags & ICMP_PROBE)
	{
		if (traceroute->fd[SO_SND] > 0 && close(traceroute->fd[SO_SND]) < 0)
		{
			error("close_all icmp", errno, TRUE);
		}
		traceroute->fd[SO_SND] = -1;
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			if (traceroute->fd[i] > 0)
			{
				if (close(traceroute->fd[i]) < 0)
				{
					er = errno;
				}
				traceroute->fd[i] = -1;
			}
		}
	}
	if (er)
		error("close_all", er, TRUE);
}

void create_sockets(t_traceroute *traceroute)
{
	close_all(traceroute);
	if ((traceroute->flags & ICMP_PROBE && (traceroute->fd[SO_SND] = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) <= -1)
		|| ((traceroute->flags & ICMP_PROBE) == 0 && (traceroute->fd[SO_SND] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= -1))
		error("create send socket", errno, TRUE);
	if (traceroute->flags & ICMP_PROBE)
	{
		traceroute->fd[SO_RECV] = traceroute->fd[SO_SND];
	}
	else if ((traceroute->fd[SO_RECV] = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) <= -1)
	{
		error("create recv socket", errno, FALSE);
		close_all(traceroute);
	}
}

void resolve_send_wait(const t_traceroute traceroute)
{
	if (traceroute.send_wait <= 0)
	{
		return;
	}
	if (traceroute.send_wait < 10)
		sleep((unsigned int)traceroute.send_wait);
	else
		usleep((unsigned int)(traceroute.send_wait * 1000));
}

char	*build_packet(t_traceroute *traceroute)
{
	char *packet = NULL;
	if (traceroute->data_size != 0 && (packet = malloc(traceroute->data_size)) == NULL)
	{
		close_all(traceroute);
		error("malloc", errno, TRUE);
	}
	if (traceroute->data_size > 0)
		ft_memset(packet, 'A', traceroute->data_size);
	return packet;
}

void print_full(char *prev_addr, const double end_count, char *temp, char *from)
{
	if (ft_strncmp(prev_addr, temp, strlen(temp)))
	{
		ft_printf("%s (%s)  %.3f ms ", from, temp, end_count);
	}
	else
		ft_printf(" %.3f ms ", end_count);
}

void print_no_resolve(char *prev_addr, const double end_count, char *temp)
{
	if (ft_strncmp(prev_addr, temp, strlen(temp)))
	{
		ft_printf("%s  %.3f ms ", temp, end_count);
	}
	else
		ft_printf(" %.3f ms ", end_count);
}

void finalize(t_traceroute traceroute, char **prev_addr, int queries, char *temp)
{
	free(*prev_addr);
	*prev_addr = temp;
	if (queries < traceroute.n_queries - 1)
		resolve_send_wait(traceroute);
	if (queries == traceroute.n_queries - 1)
	{
		free(*prev_addr);
		*prev_addr = NULL;
	}
}

void save_address(struct sockaddr_in recv_addr, char **temp)
{
	char *ntoa_res = inet_ntoa(recv_addr.sin_addr);
	*temp = malloc(ft_strlen(ntoa_res) + 1);
	ft_memcpy(*temp, ntoa_res, ft_strlen(ntoa_res) + 1);
}

void set_timeout(t_traceroute traceroute)
{
	struct timeval timeout = {1, 1 * 100000};
	if (setsockopt(traceroute.fd[SO_RECV], SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		close_all(&traceroute);
		error("setsockopt timeout", errno, TRUE);
	}
}

void apply_icmp_filters(struct icmp_filter *filter)
{
	icmp_filter_set_block_all(filter);
	icmp_filter_set_pass(ICMP_ECHOREPLY, filter);
	icmp_filter_set_pass(ICMP_TIME_EXCEEDED, filter);
	icmp_filter_set_pass(ICMP_DEST_UNREACH, filter);
	icmp_filter_set_pass(ICMP_REDIRECT, filter);
}

void display_errors(const struct icmphdr *r_icmp_hdr)
{
	char error_buffer[255];
	int r_nb_packet = 0;
	analyze_packet(r_icmp_hdr, &r_nb_packet, error_buffer);
	error(error_buffer, -1, FALSE);
}

int main(int const ac, char **av)
{
	char				buffer[ADDR_LEN];
	char				*source;
	uint8_t				must_do = 0;
	t_traceroute		traceroute;
	struct sockaddr_in	dest_addr;
	struct icmp_filter  filter;

	ft_memset(buffer, 0, ADDR_LEN);
	traceroute = check_args(ac, av, buffer);
	if (getuid() != 0)
	{
		ft_fprintf(STDERR_FILENO, "Error in privilege, are you root ?\n");
		return 1;
	}
	ft_memset(&dest_addr, 0, sizeof(dest_addr));
	if ((source = is_valid_ip(buffer, &dest_addr)) == NULL)
	{
		close_all(&traceroute);
		error("unknown host", -1, TRUE);
	}
	if (traceroute.flags & ICMP_PROBE)
	{
		apply_icmp_filters(&filter);
	}
	ft_printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", buffer, source, traceroute.max_ttl, traceroute.packet_size);
	free(source);
	source = NULL;
	for (int ttl = traceroute.first_ttl; ttl <= traceroute.max_ttl && !must_do; ttl++)
	{
		ft_printf("%s%d  ", ttl >= 10 ? "" : " ", ttl);
		char *prev_addr = NULL;
		for (int queries = 0; queries < traceroute.n_queries; queries++, traceroute.port++)
		{
			create_sockets(&traceroute);
			if (traceroute.flags & ICMP_PROBE)
			{
				if (setsockopt(traceroute.fd[SO_SND], SOL_RAW, ICMP_FILTER, &filter, sizeof(filter)) == -1)
				{
					close_all(&traceroute);
					error("setsockopt icmp filter", errno, TRUE);
				}
			}
			dest_addr.sin_port = htons(traceroute.port);
			if (setsockopt(traceroute.fd[SO_SND], IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
			{
				close_all(&traceroute);
				error("setsockopt ttl", errno, TRUE);
			}
			if (traceroute.flags & ICMP_PROBE)
			{
				static int	nb_packets;
				t_icmp_packet	icmp_hdr;
				icmp_hdr = prepare_icmp_packet(&nb_packets);
				if (sendto(traceroute.fd[SO_SND], &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
				{
					close_all(&traceroute);
					error("sento", errno, TRUE);
				}
			}
			else
			{
				char	*packet = build_packet(&traceroute);
				if (sendto(traceroute.fd[SO_SND], packet, packet ? sizeof(packet): 0, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
				{
					if (packet)
						free(packet);
					close_all(&traceroute);
					error("sento", errno, TRUE);
				}
				free(packet);
				packet = NULL;
			}
			const double  start_count = gettimeinms();
			set_timeout(traceroute);
			char response[1024];
			struct sockaddr_in	recv_addr;
			socklen_t			recv_len = sizeof(recv_addr);
			ft_memset(&response, 0, 1024);
			if (recvfrom(traceroute.fd[SO_RECV], response, sizeof(response), 0, (struct sockaddr *)&recv_addr, &recv_len) <= 0)
			{
				ft_printf("* ");
				continue;
			}
			const double  end_count = gettimeinms() - start_count;
			const struct icmphdr *r_icmp_hdr = (struct icmphdr *) (response + sizeof(struct iphdr));
			char *temp = NULL;
			save_address(recv_addr, &temp);
			if (r_icmp_hdr->type == ICMP_TIME_EXCEEDED
				|| r_icmp_hdr->type == ICMP_DEST_UNREACH
				|| (traceroute.flags & ICMP_PROBE && r_icmp_hdr->type == ICMP_ECHOREPLY))
			{
				if (traceroute.flags & NO_RESOLVE)
				{
					print_no_resolve(prev_addr, end_count, temp);
				}
				else
				{
					char from[NI_MAXHOST];
					ft_memset(from, 0, NI_MAXHOST);
					const int i = getnameinfo((struct sockaddr *) &recv_addr, recv_len, from, NI_MAXHOST, NULL, 0, 0);
					if (i != 0)
						ft_printf("reverse dns: %s", gai_strerror(i));
					print_full(prev_addr, end_count, temp, from);
				}
				if (r_icmp_hdr->type == ICMP_DEST_UNREACH
					|| (traceroute.flags & ICMP_PROBE && r_icmp_hdr->type == ICMP_ECHOREPLY))
				{
					must_do = 1;
				}
			}
			else
			{
				display_errors(r_icmp_hdr);
			}
			finalize(traceroute, &prev_addr, queries, temp);
		}
		ft_printf("\n");
		if (ttl < traceroute.max_ttl && must_do == 0)
		{
			resolve_send_wait(traceroute);
		}
	}
	close_all(&traceroute);
	return 0;
}
