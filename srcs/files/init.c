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

void	safe_atoi(const char *str, int *i)
{
	int		res = 0;
	char	*verif = NULL;

	res = ft_atoi(str);
	verif = ft_itoa(res);
	if (ft_strncmp(verif, str, ft_strlen(verif)))
	{
		error("Invalid value, falling back to default", -1, FALSE);
		free(verif);
		verif = NULL;
		return;
	}
	free(verif);
	verif = NULL;
	*i = res;
}

t_traceroute	check_args(const int ac, char **av, char *buffer)
{
	size_t  len = 0;

	if (ac == 1)
		print_help();
	t_traceroute traceroute =
	{
		0,
		{-1, -1},
		30,
		0,
		3,
		1,
		60,
		32,
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
						safe_atoi(*av, &traceroute.first_ttl);
						if (traceroute.first_ttl > 255)
							error("max hops cannot be more than 255", -1, TRUE);
						if (traceroute.first_ttl < 1)
							error("first hop out of range", -1, TRUE);
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
						safe_atoi(*av, &traceroute.max_ttl);
						if (traceroute.max_ttl > 255)
							error("max hops cannot be more than 255", -1, TRUE);
						if (traceroute.max_ttl < 1)
							error("first hop out of range", -1, TRUE);
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
						fprintf(stderr, RED"Bad option \'--%s\'\n"RESET, *av);
						exit(2);
					default:
						fprintf(stderr, RED"Bad option \'-%s\'\n"RESET, *av);
						exit(2);
				}
				++*av;
			}
		}
		else
		{
			if (ft_strlen(buffer))
			{
				const size_t size = ft_strlen(*av);
				for (size_t i = 0; i < size; i++)
				{
					if (!ft_isdigit((*av)[i]))
					{
						fprintf(stderr, RED"Cannot handle \"%s\" cmdline arg\n"RESET, *av);
						exit(2);
					}
				}
				safe_atoi(*av, &traceroute.packet_size);
				if (traceroute.packet_size > 65000)
				{
					fprintf(stderr, RED"too big packetlen %d specified"RESET, traceroute.packet_size);
					exit(2);
				}
				if (traceroute.packet_size < 28)
				{
					traceroute.data_size = 0;
					traceroute.packet_size = 28;
				}
				else
				{
					traceroute.data_size = traceroute.packet_size - 28;
				}
			}
			else
			{
				size_t const addr_len = ft_strlen(*av);
				ft_memcpy(buffer, *av, addr_len > ADDR_LEN ? ADDR_LEN : addr_len);
			}
		}
		++av;
	}
	if (buffer[0] == '\0')
		error("Specify \"host\" missing argument.", -1, TRUE);
	if (traceroute.max_ttl < traceroute.first_ttl)
		error("first hop out of range", -1, TRUE);
	return traceroute;
}
