/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/16 12:13:50 by wester        #+#    #+#                 */
/*   Updated: 2021/02/16 16:36:38 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

client::client(int sockfd): newfd(0), their_addr(), addr_size(sizeof(their_addr))
{
    bzero(&their_addr, addr_size);
    newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
    if (newfd == -1){
        perror("accept");
        exit(1);
    }
    printf("newfd: %d\n", newfd);
}

client::client(const client& other)
{
    *this = other;
}

client&     client::operator=(const client& other)
{
    this->newfd = other.newfd;
    this->their_addr = other.their_addr;
    this->addr_size = other.addr_size;
    return *this;
}

client::~client(){}