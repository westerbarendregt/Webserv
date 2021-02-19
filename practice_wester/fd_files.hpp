/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   fd_files.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/18 13:56:35 by wester        #+#    #+#                 */
/*   Updated: 2021/02/18 15:51:37 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef FD_FILES_HPP
# define FD_FILES_HPP

# include <sys/time.h>
# include <iostream>

class fd_files
{
  public:
    fd_set  master;
    fd_set  read_fds;
    int     fdmax;
    struct timeval tv;
    
    fd_files();
    fd_files(const fd_files& other);
    fd_files&       operator=(const fd_files& other);
    ~fd_files();
};

#endif