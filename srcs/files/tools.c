//
// Created by tnoulens on 12/27/23.
//

#include "traceroute.h"

unsigned short calculate_checksum(unsigned short *buf, int len)
{
	unsigned long   sum = 0;
	unsigned short  *w = buf;
	unsigned short  answer = 0;

	while (len > 1)
	{
		sum += *w++;
		len -= 2;
	}
	if (len == 1)
	{
		*(unsigned char *)&answer = *(unsigned char *)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += sum >> 16;
	answer = ~sum;
	return answer;
}

void    print_reply(const struct icmphdr *r_icmp_hdr, const char *r_buffer)
{
	printf(" info from ECHO REPLY:\n");
	printf(" type: %d\n", r_icmp_hdr->type);
	printf(" code: %d\n", r_icmp_hdr->code);
	printf(" id: %d\n", ntohs(r_icmp_hdr->un.echo.id));
	printf(" sequence: %d\n", ntohs(r_icmp_hdr->un.echo.sequence));
	printf(" data: %s\n", r_buffer);
}

double gettimeinms(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		fprintf(stderr, "Error in getting start time\n");
		return (-1);
	}
	return (double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0;
}

// Function to set all bits in the filter
// Function to set all bits in the filter (block all)
void    icmp_filter_set_block_all(struct icmp_filter *filter)
{
	filter->data = ~0U;
}

// Function to clear a specific bit (type) in the filter (allow pass)
void    icmp_filter_set_pass(int type, struct icmp_filter *filter)
{
	if (type < 32)
		filter->data &= ~(1U << type);
}

void    print_help(void)
{
	ft_putendl_fd("Usage:\n	ft_traceroute [ -I ] [ -f first_ttl ] [ -m max_ttl ] [ -p port ] [ -q nqueries ] [ -z sendwait ] host", STDOUT_FILENO);
	ft_putendl_fd("Options:", STDOUT_FILENO);
	ft_putendl_fd("  -f first_ttl                Start from the first_ttl hop (instead from 1)", STDOUT_FILENO);
	ft_putendl_fd("  -I                          Use ICMP ECHO for tracerouting", STDOUT_FILENO);
	ft_putendl_fd("  -m max_ttl                  Set the max number of hops (max TTL to be", STDOUT_FILENO);
	ft_putendl_fd("                              reached). Default is 30", STDOUT_FILENO);
	ft_putendl_fd("  -n                          Do not resolve IP addresses to their domain names", STDOUT_FILENO);
	ft_putendl_fd("  -p port                     Set the destination port to use. It is either", STDOUT_FILENO);
	ft_putendl_fd("                              initial udp port value for \"default\" method", STDOUT_FILENO);
	ft_putendl_fd("                              (incremented by each probe, default is 33434), or", STDOUT_FILENO);
	ft_putendl_fd("                              initial seq for \"icmp\" (incremented as well,", STDOUT_FILENO);
	ft_putendl_fd("                              default from 1)", STDOUT_FILENO);
	ft_putendl_fd("  -q nqueries                 Set the number of probes per each hop. Default is", STDOUT_FILENO);
	ft_putendl_fd("                              3", STDOUT_FILENO);
	ft_putendl_fd("  -z sendwait                 Minimal time interval between probes (default 0).", STDOUT_FILENO);
	ft_putendl_fd("                              If the value is more than 10, then it specifies a", STDOUT_FILENO);
	ft_putendl_fd("                              number in milliseconds, else it is a number of", STDOUT_FILENO);
	ft_putendl_fd("                              seconds (float point values allowed too)", STDOUT_FILENO);
	ft_putendl_fd("  --help                      Read this help and exit", STDOUT_FILENO);
	ft_putendl_fd("", STDOUT_FILENO);
	ft_putendl_fd("Arguments:", STDOUT_FILENO);
	ft_putendl_fd("+     host          The host to traceroute to", STDOUT_FILENO);
	ft_putendl_fd("      packetlen     The full packet length (default is the length of an IP", STDOUT_FILENO);
	ft_putendl_fd("                    header plus 40). Can be ignored or increased to a minimal", STDOUT_FILENO);
	ft_putendl_fd("                    allowed value", STDOUT_FILENO);
	exit(0);
}
