/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   client.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/16 11:42:36 by wester        #+#    #+#                 */
/*   Updated: 2021/02/16 15:06:48 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "address.hpp"

class client
{
    client();
    int newfd;
    struct sockaddr_in their_addr;
    socklen_t addr_size;
    
  public:
    client(int sockfd);
    client(const client& other);
    client& operator=(const client& other);
    ~client();
    
};

#endif