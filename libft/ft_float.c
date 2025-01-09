//
// Created by tnoulens on 1/7/25.
//
#include "libft.h"

int	ft_float(double nbr, int precision)
{
	int		j;

	j = ft_putfloat_fd(nbr, precision, STDOUT_FILENO);
	return (j);
}
