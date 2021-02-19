/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: totartar <totartar@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/19 22:47:36 by totartar      #+#    #+#                 */
/*   Updated: 2021/02/19 22:47:37 by totartar      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <string>//std::string
#include <string.h>//memset
#include <unistd.h>//size_t

size_t		write_reverse(size_t n, char *buf)
{
	size_t	len;

	len = 0;
	while (n)
	{
		buf[len] = '0' + n % 10;
		n *= 0.1;
		len++;
	}
	return (len);
}

std::string sputnbr(size_t n)
{
	size_t	start;
	size_t	end;
	char	buf[20];
	char	swap;

	memset(buf, 0, 20);
	end = 0;
	if (!n)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return (std::string(buf));
	}
	start = write_reverse(n, buf);
	buf[start] = '\0';
	start--;
	while (start > end)
	{
		swap = buf[end];
		buf[end] = buf[start];
		buf[start] = swap;
		start--;
		end++;
	}
	return (std::string(buf));
}
