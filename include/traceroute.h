
#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <net/if.h>
# include <netinet/in.h>
# include <netdb.h>
# include <ifaddrs.h>
# include <linux/if_ether.h>
# include <signal.h>
# include <errno.h>
# include "colors.h"
# include "libft.h"
# include <netpacket/packet.h>
# include <net/ethernet.h>
# include <netinet/if_ether.h>
# include <netinet/ip.h>
# include <sys/ioctl.h>
# include <sys/time.h>
# include <linux/icmp.h>

# define PING_PKT_S 64
# define PING_USLEEP_RATE 1
# define RECV_TIMEOUT 1
# define ADDR_LEN 1024
# define M 1000000
# define THE_MAX 9223372036854775807.0
# define MAX_HOP 30
# define DEFAULT_PACKET "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

/*
 * -f first_ttl, --first=first_ttl
 * 		Specifies with what TTL to start. Defaults to 1.
 *
 * -I, --icmp
 * 		Use ICMP ECHO for probes
 *
 * -m max_ttl, --max-hops=max_ttl
 * 		Specifies  the  maximum number of hops (max time-to-live value) traceroute will probe. The
 * 		default is 30.
 *
 * 	-p port, --port=port
 *		For UDP tracing, specifies the destination port base traceroute will use (the  destination
 *		port number will be incremented by each probe).
 *		For  ICMP  tracing,  specifies  the initial ICMP sequence value (incremented by each probe
 *		too).
 *		For TCP and others specifies just the (constant) destination port to connect.  When  using
 *		the tcptraceroute wrapper, -p specifies the source port.
 *
 *	-q nqueries, --queries=nqueries
 *		Sets the number of probe packets per hop. The default is 3.
 *
 *	-z sendwait, --sendwait=sendwait
 *		Minimal time interval between probes (default 0).  If the value is more than 10,  then  it
 *		specifies a number in milliseconds, else it is a number of seconds (float point values al‐
 *		lowed too).  Useful when some routers use rate-limit for ICMP messages.
 *
 * */

typedef enum e_fd
{
	SO_RECV,
	SO_SND
}	t_fd;

typedef enum e_flags
{
	ICMP_PROBE = 0b00000001,
	NO_RESOLVE = 0b00000010
}   t_flags;

typedef struct s_traceroute
{
	int		flags; // I and n
	int		fd[2];
	int		max_ttl; // m
	float	send_wait; // z
	int		n_queries; // q
	int		first_ttl; // f
	int		packet_size; // 2nd arg
	int		data_size;
	ushort	port;
}	t_traceroute;

typedef struct s_icmp_packet
{
	struct icmphdr  hdr;
	char            msg[PING_PKT_S - sizeof(struct icmphdr)];
}   t_icmp_packet;

void            error(const char *msg, int error_code, int must_exit);
char			*is_valid_ip(char *ip, struct sockaddr_in *data);
t_traceroute	check_args(int ac, char **av, char *buffer);
t_icmp_packet	prepare_icmp_packet(int *nb_packets);
void            print_reply(const struct icmphdr *r_icmp_hdr, const char *r_buffer);
void            analyze_packet(const struct icmphdr *r_icmp_hdr, int *nb_r_packets, char *error_buffer);
void            print_help(void);
unsigned short  calculate_checksum(unsigned short *buf, int len);
double          gettimeinms(void);
void            icmp_filter_set_pass(int type, struct icmp_filter *filter);
void            icmp_filter_set_block_all(struct icmp_filter *filter);

# endif /* !TRACEROUTE_H */
