/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   address.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/15 11:21:06 by wester        #+#    #+#                 */
/*   Updated: 2021/02/19 11:13:20 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "address.hpp"

address::address()
{
    data.sin_family = AF_INET;
    data.sin_port = htons(MYPORT);
    data.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(data.sin_zero, '\0', sizeof(data.sin_zero));
}

address::~address()
{}

const struct sockaddr*    address::getData()
{
    return (const struct sockaddr *)&data;
}

socklen_t                 address::getSize()
{
    return sizeof(data);
}