//
// Created by tnoulens on 12/26/23.
//

#include "traceroute.h"

void capped_ttl(char **av, int *ttl)
{
	int status = 0;
	long long const temp = ft_capped_atoll(*av, &status);
	if (status || temp < 0)
		error("first hop out of range", -1, TRUE);
	if (temp > 255)
		error("max hops cannot be more than 255", -1, TRUE);
	*ttl = (int)temp;
}

void capped_port(char **av, ushort *port)
{
	int status = 0;
	long long const temp = ft_capped_atoll(*av, &status);
	if (status || temp < 0 || temp > UINT16_MAX)
		error("invalid port", -1, TRUE);
	*port = (ushort)temp;
}

void capped_queries(char **av, int *q)
{
	int status = 0;
	long long const temp = ft_capped_atoll(*av, &status);
	if (status || temp < 1 || temp > 10)
		error("no more than 10 probes per hop", -1, TRUE);
	*q = (int)temp;
}

// f I m p q z
t_traceroute	check_args(const int ac, char **av, char *buffer)
{
	size_t  len = 0;

	if (ac == 1)
		print_help();
	ft_bzero(buffer, ADDR_LEN);
	t_traceroute traceroute =
	{
		0,
		{ -1, -1, -1 },
		30,
		0,
		3,
		1,
		33434
	};
	++av;
	len = ft_ptrlen(av);
	while (len--)
	{
		if (**av == '-')
		{
			++*av;
			while (**av)
			{
				switch (**av)
				{
					case 'f':
						++*av;
						if (**av == '\0')
						{
							++av;
							--len;
						}
						capped_ttl(av, &traceroute.first_ttl);
						while (*(*av + 1))
							++*av;
						break;
					case 'I':
						traceroute.flags |= ICMP_PROBE;
						break ;
					case 'n':
						traceroute.flags |= NO_RESOLVE;
					break ;
					case 'm':
						++*av;
						if (**av == '\0')
						{
							++av;
							--len;
						}
						capped_ttl(av, &traceroute.max_ttl);
						while (*(*av + 1))
							++*av;
						break;
					case 'p':
						++*av;
						if (**av == '\0')
						{
							++av;
							--len;
						}
						capped_port(av, &traceroute.port);
						while (*(*av + 1))
							++*av;
						break;
					case 'q':
						++*av;
						if (**av == '\0')
						{
							++av;
							--len;
						}
						capped_queries(av, &traceroute.n_queries);
						while (*(*av + 1))
							++*av;
						break;
					case 'z':
						++*av;
					if (**av == '\0')
					{
						++av;
						--len;
					}
					traceroute.send_wait = ft_atof(*av);
					if (traceroute.send_wait < 0)
					{
						printf("bad sendtime %f specified", traceroute.send_wait);
						exit(2);
					}
					while (*(*av + 1))
						++*av;
					break;
					case '-':
						++*av;
						if (!ft_strcmp("help", *av))
						{
							print_help();
						}
						fprintf(stderr, RED"Bad option \'%c\'\n"RESET, **av);
						exit(2);
					default:
						fprintf(stderr, RED"Bad option \'%c\'\n"RESET, **av);
						exit(2);
				}
				++*av;
			}
		}
		else
		{
			if (ft_strlen(buffer))
			{
				fprintf(stderr, RED"Cannot handle \"%s\" cmdline arg\n"RESET, *av);
				exit(2);
			}
			size_t const addr_len = ft_strlen(*av);
			ft_memcpy(buffer, *av, addr_len > ADDR_LEN ? ADDR_LEN : addr_len);
		}
		++av;
	}
	if (buffer[0] == '\0')
		error("Specify \"host\" missing argument.", -1, TRUE);
	if (traceroute.max_ttl < traceroute.first_ttl)
		error("first hop out of range", -1, TRUE);
	return traceroute;
}
