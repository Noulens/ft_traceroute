/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putfloat_fd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waxxy <waxxy@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/24 13:37:50 by waxxy             #+#    #+#             */
/*   Updated: 2023/01/24 13:37:54 by waxxy            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int ft_recursive_power(int n, int power)
{
    if (power < 0)
        return (0);
    if (power == 0)
        return (1);
    return (n * ft_recursive_power(n, power - 1));
}

static int ft_nblen(int len, unsigned long nb)
{
    while (nb > 0)
    {
        ++len;
        nb /= 10;
    }
    return (len > 0 ? len : 1);
}

static void ft_putter(long n, int prec, int fd, int *len)
{
    if (n < 0)
    {
        write(fd, "-", 1);
        *len += 1;
        n = -n;
    }

    long integer_part = n / ft_recursive_power(10, prec);
    long fractional_part = n % ft_recursive_power(10, prec);
    if (integer_part == 0)
    {
        write(fd, "0", 1);
        *len += 1;
    }
    else
    {
        char int_part[20];
        int j = 0;
        while (integer_part > 0)
        {
            int_part[j++] = (integer_part % 10) + '0';
            integer_part /= 10;
        }
        while (--j >= 0)
            *len += write(fd, &int_part[j], 1);
    }
    if (prec > 0)
    {
        write(fd, ".", 1);
        *len += 1;
        int frac_digits = ft_nblen(0, fractional_part);
        for (int k = 0; k < prec - frac_digits; ++k)
        {
            write(fd, "0", 1);
            *len += 1;
        }
        char frac_part[20];
        int k = 0;
        while (fractional_part > 0)
        {
            frac_part[k++] = (fractional_part % 10) + '0';
            fractional_part /= 10;
        }
        while (--k >= 0)
            *len += write(fd, &frac_part[k], 1);
    }
}

int ft_putfloat_fd(float x, int precision, int fd)
{
    long n;
    int pow;
    int res = 0;

    if (precision < 0)
        return (write(2, "invalid precision\n", 18), 0);
    pow = ft_recursive_power(10, precision);
    x = x * pow + (x < 0 ? -0.5 : 0.5);
    if (x > 2147483647.0f || x < -2147483648.0f)
        return (write(2, "overflow\n", 9), 0);
    n = (long)x;
    ft_putter(n, precision, fd, &res);
    return (res);
}

/*
#include <unistd.h>


int	main(void)
{
	ft_putfloat_fd(0.0f, 3, 1);
	return (0);
}*/
