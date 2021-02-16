/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   create.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/13 11:40:56 by wester        #+#    #+#                 */
/*   Updated: 2021/02/16 16:37:53 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "create.hpp"
#include "address.hpp"
#include "client.hpp"

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
    std::cout << "hello" << std::endl;
    
    int yes;

    // // info = getaddrinfo();

    // // std::cout << info->ai_flags << std::endl;

    int sockfd = 0;
    int bind_num;
    int listen_num;
    int backlog = 20;
    int new_fd;
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    bind_num = bind(sockfd, my_addr.getData(), my_addr.getSize());
    // bind_num = bind(sockfd, (sockaddr*)&hint, sizeof(hint));
    if (bind_num == -1)
    {
        perror("binding");
        exit(1);
        
    }
    
    printf("%d  %d\n", sockfd, bind_num);
    if (listen(sockfd, backlog) == -1){
        perror("listen");
        exit(1);
    }
    // fcntl(sockfd, F_SETFL, O_NONBLOCK);
    client newclient(sockfd);
    // new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        
    // if (setsockopt(MYPORT, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
    //     perror("setsockopt");
    //     exit(1);
    // }
    
    
    
    return 0;
}