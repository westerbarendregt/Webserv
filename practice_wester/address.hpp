/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   address.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/15 11:17:41 by wester        #+#    #+#                 */
/*   Updated: 2021/02/16 15:17:48 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef ADDRESS_HPP
# define ADDRESS_HPP

# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdio.h>
# include <netinet/in.h>
# include <strings.h>
# include <fcntl.h>
# include <unistd.h>
# define MYPORT 3490

class address
{
    struct sockaddr_in data;

    address(const address& other);
    address& operator=(const address& other);
    
  public:
    address();
    ~address();
    const struct sockaddr*    getData();
    socklen_t                 getSize();
};

#endif