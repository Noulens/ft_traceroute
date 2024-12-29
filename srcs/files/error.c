#include "traceroute.h"

void    error(const char *msg, int error_code, int must_exit)
{
	if (error_code != -1)
		fprintf(stderr, RED"ft_traceroute: %s: %s\n"RESET, msg, strerror(error_code));
	else
		fprintf(stderr, RED"ft_traceroute: %s\n"RESET, msg);
	if (must_exit == TRUE && error_code == -1)
		exit(EXIT_FAILURE);
	if (must_exit == TRUE)
		exit (error_code);
}
