/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/16 12:13:50 by wester        #+#    #+#                 */
/*   Updated: 2021/02/19 14:30:15 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

client::client(int sockfd, fd_files& fd): newfd(0), their_addr(), addr_size(sizeof(their_addr))
{
    bzero(&their_addr, addr_size);
    newfd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
    if (newfd == -1){
        perror("accept");
        // exit(1);
    }
    else
    {
        FD_SET(newfd, &(fd.master));
        if (newfd > fd.fdmax)
            fd.fdmax = newfd;
        // std::cout << "1.fdmax: " << fd.fdmax << std::endl;

    }
    
    char msg_get[1000];
    int ret = 1000;
    while (ret == 1000)
    {
        ret = recv(newfd, &msg_get, 1000, 0);
        if (ret == -1)
        {
            perror("send");
            exit(1);
        }
        msg_get[ret] = 0;
        printf("%s\n", msg_get);
        // do_something_with_info(msg_get);
        close(newfd);
    }
    // char *msg_send = "sending first message\n";
    // int ret = -1;
    // while (ret != strlen(msg_send))
    // {
    //     ret = send(newfd, msg_send, strlen(msg_send), 0);
    //     if (ret == -1)
    //     {
    //         perror("send")
    //         exit(1);
    //     }
    //     msg = this->cutStr(msg_send, ret);
    // }
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

char*       client::cutStr(char* msg, int num)
{
    int i = 0;

    for (; msg[i + num]; ++i)
        msg[i] = msg[i + num];
    msg[i] = 0;
    return msg;
}