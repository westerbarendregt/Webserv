/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   create.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/13 11:40:56 by wester        #+#    #+#                 */
/*   Updated: 2021/02/19 14:29:14 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "create.hpp"
#include "address.hpp"
#include "client.hpp"
#include "fd_files.hpp"

int         main(void)
{
    // struct addrinfo *info;
    // struct sockaddr_in their_addr;

    // socklen_t addr_size;
    // addr_size = sizeof(their_addr);

    // bzero(&their_addr, sizeof(their_addr));
    address my_addr;
    // struct sockaddr_in hint;
    // hint.sin_family = AF_INET;
    // hint.sin_port = htons(MYPORT);
    // hint.sin_addr.s_addr = inet_addr("10.12.110.57");
    
    int yes;

    // // info = getaddrinfo();

    // // std::cout << info->ai_flags << std::endl;

    int sockfd = 0;
    int bind_num;
    int listen_num;
    int backlog = 20;
    int new_fd;
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    bind_num = bind(sockfd, my_addr.getData(), my_addr.getSize());
    // bind_num = bind(sockfd, (sockaddr*)&hint, sizeof(hint));
    if (bind_num == -1)
    {
        close(sockfd);
        perror("binding");
        exit(1);
        
    }
    
    // printf("%d  %d\n", sockfd, bind_num);
    if (listen(sockfd, backlog) == -1)
    {
        perror("listen");
        exit(1);
    }
    
    fd_files fd;

    FD_SET(sockfd, &(fd.master));
    fd.fdmax = sockfd;
    int nbytes;
    char buf[256];
    
    for (;;)
    {
        fd.read_fds = fd.master;
        std::cout << "Waiting for connecting..." << std::endl;
        if (select(fd.fdmax + 1, &fd.read_fds, NULL, NULL, &(fd.tv)) == -1)
        {
            perror("select");
            exit(1);
        }
        // std::cout << "fdmax: " << fd.fdmax << std::endl;
        for (int i = 0; i <= fd.fdmax; ++i)
        {
            if (FD_ISSET(i, &(fd.read_fds)))
            {
                if (i == sockfd)
                {
                    std::cout << "client if" << std::endl;
                    // fcntl(sockfd, F_SETFL, O_NONBLOCK);
                    client newclient(sockfd, fd);
                
                }
                else
                {
                    std::cout << "------big else-------" << std::endl;
                    // if ((nbytes = recv(i, buf, 256, 0)) <= 0)
                    // {
                    //     if (nbytes == 0)
                    //         printf("selectserver: socket %d hung up\n", i);
                    //     else    
                    //         perror("recv");
                        close(i);
                        FD_CLR(i, &(fd.master));
                    // }
                    // else
                    // {
                    //     for (int j = 0; j <= fd.fdmax; ++j)
                    //     {
                    //         if (j != sockfd && j != i)
                    //             if (send(j, buf, nbytes, 0) == -1)
                    //                 perror("send");
                    //     }
                    // }
                }
            }
        }
    }
    
    // new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        
    // if (setsockopt(MYPORT, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
    //     perror("setsockopt");
    //     exit(1);
    // }
    
    
    
    return 0;
}
