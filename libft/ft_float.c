//
// Created by tnoulens on 1/7/25.
//
#include "libft.h"

int	ft_float(double nbr, int precision)
{
	int		j;

    if (precision == 0)
      j = ft_di((int)nbr);
    else
		j = ft_putfloat_fd(nbr, precision, STDOUT_FILENO);
	return (j);
}
