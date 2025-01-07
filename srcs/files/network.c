#include "traceroute.h"

char    *hostnameResolution(const char *hostname)
{
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *r;
	int status;
	char buffer[INET_ADDRSTRLEN];
	char from[NI_MAXHOST];
	char *ret = NULL;

	ft_memset(&hints, 0, sizeof(hints));
	ft_memset(buffer, 0, INET_ADDRSTRLEN);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	status = getaddrinfo(hostname, 0, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return NULL;
	}
	r = res;
	while (r != NULL)
	{
		if (r->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)r->ai_addr;
			if (!inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer))
				return fprintf(stderr, "inet_ntop() failed: %s", strerror(errno)), NULL;
			// Find FQDN from result of inet_ntop()
			status = getnameinfo((struct sockaddr *)(ipv4), sizeof(struct sockaddr_in), from, NI_MAXHOST, NULL, 0, 0);
			if (status != 0)
			{
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
				return NULL;
			}
			break ;
		}
		r = r->ai_next;
	}
	if (!r)
		fprintf(stderr, "No AF_INET family found in ai LL\n");
	freeaddrinfo(res);
	ret = ft_strdup(buffer);
	if (!ret)
		error("hostname resolution", errno, TRUE);
	return ret;
}

char	*is_valid_ip(char *ip, struct sockaddr_in *data)
{
	char    *source = NULL;

	if (ft_strlen(ip) == 0)
		error("usage error: Destination address required", -1, TRUE);
	for (int i = 0; ip[i]; i++)
	{
		if (ft_isalpha(ip[i]))
		{
			source = hostnameResolution(ip);
			if (!source)
				return NULL;
			break ;
		}
		if (ip[i + 1] == '\0')
		{
			source = ft_strdup(ip);
			if (!source)
				error("is_valid_ip", errno, TRUE);
		}
	}
	if (ft_strnstr(source, ".", ft_strlen(source)) == NULL)
	{
		char    *test = ft_itoa(ft_atoi(source));
		if (!ft_strcmp(test, source))
		{
			data->sin_family = AF_INET;
			data->sin_addr.s_addr = htonl(ft_atoi(source));
			free(test);
		}
		else
			return free(source), free(test), NULL;
	}
	else
	{
		if (inet_pton(AF_INET, source, &data->sin_addr) != 1)
			return free(source), NULL;
	}
	data->sin_family = AF_INET;
	return source;
}

t_icmp_packet	prepare_packet(int *nb_packets)
{
	int i;
	t_icmp_packet icmp_hdr;

	ft_memset(&icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr.hdr.type = ICMP_ECHO;
	icmp_hdr.hdr.un.echo.id = htons(getpid());
	icmp_hdr.hdr.code = 0;
	for (i = 0; i < (int)sizeof(icmp_hdr.msg) - 1; i++)
		icmp_hdr.msg[i] = 'A';
	icmp_hdr.msg[i] = '\0';
	icmp_hdr.hdr.un.echo.sequence = htons((*nb_packets)++);
	icmp_hdr.hdr.checksum  = calculate_checksum((uint16_t *)&icmp_hdr, sizeof(icmp_hdr));
	return icmp_hdr;
}

void    analyze_packet(const struct icmphdr *r_icmp_hdr, int *nb_r_packets, char *error_buffer)
{
	switch (r_icmp_hdr->type)
	{
		case ICMP_ECHOREPLY:
			if (r_icmp_hdr->code == 0)
			{
				(*nb_r_packets)++;
			}
			else
				sprintf(error_buffer, "Unhandled ECHOREPLY CODE: %d", r_icmp_hdr->code);
			break ;
		case ICMP_TIME_EXCEEDED:
			switch (r_icmp_hdr->code)
			{
				case ICMP_EXC_TTL:
					sprintf(error_buffer, "Time to live exceeded");
					break ;
				case ICMP_EXC_FRAGTIME:
					sprintf(error_buffer, "Fragment Reass time exceeded");
					break ;
			}
			break ;
		case ICMP_DEST_UNREACH:
			switch (r_icmp_hdr->code)
			{
				case ICMP_NET_UNREACH:
					sprintf(error_buffer, "Network Unreachable");
					break ;
				case ICMP_HOST_UNREACH:
					sprintf(error_buffer, "Host Unreachable");
					break ;
				case ICMP_PROT_UNREACH:
					sprintf(error_buffer, "Protocol Unreachable");
					break ;
				case ICMP_PORT_UNREACH:
					sprintf(error_buffer, "Port Unreachable");
					break ;
				case ICMP_FRAG_NEEDED:
					sprintf(error_buffer, "Fragmentation Needed/DF set");
					break ;
				case ICMP_SR_FAILED:
					sprintf(error_buffer, "Source Route failed");
					break ;
				case ICMP_PKT_FILTERED:
					sprintf(error_buffer, "Packet filtered");
					break ;
				case ICMP_PREC_VIOLATION:
					sprintf(error_buffer, "Precedence violation");
					break ;
				case ICMP_PREC_CUTOFF:
					sprintf(error_buffer, "Precedence cut off");
					break ;
				default:
					sprintf(error_buffer, "Unknown error: %d", r_icmp_hdr->code);
					break ;
			}
			break ;
		case ICMP_REDIRECT:
			switch (r_icmp_hdr->code)
			{
				case ICMP_REDIR_NET:
					sprintf(error_buffer, "Redirect Net");
					break ;
				case ICMP_REDIR_HOST:
					sprintf(error_buffer, "Redirect Host");
					break ;
				case ICMP_REDIR_NETTOS:
					sprintf(error_buffer, "Redirect Net for TOS");
					break ;
				case ICMP_REDIR_HOSTTOS:
					sprintf(error_buffer, "Redirect Host for TOS");
					break ;
				default:
					sprintf(error_buffer, "Unknown error: %d", r_icmp_hdr->code);
					break ;
			}
			break ;
		default:
			sprintf(error_buffer, "Unhandled ICMP TYPE: %d", r_icmp_hdr->type);
			break ;
	}
}
