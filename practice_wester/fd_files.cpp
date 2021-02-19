/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fd_files.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/18 13:59:21 by wester        #+#    #+#                 */
/*   Updated: 2021/02/18 15:48:54 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "fd_files.hpp"

fd_files::fd_files()
{
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    fdmax = 0;
    tv.tv_sec = 2;
    tv.tv_usec = 500000;
}

fd_files::fd_files(const fd_files& other)
{
    *this = other;
}

fd_files&       fd_files::operator=(const fd_files& other)
{
    this->fdmax = other.fdmax;
    this->master = other.master;
    this->read_fds = other.read_fds;
    this->tv = other.tv;
    return *this;
}

fd_files::~fd_files()
{
}
