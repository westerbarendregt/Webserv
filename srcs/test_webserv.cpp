
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include "ConfigParser.hpp"
/*
struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char * ai_canonname;
    struct sockaddr * ai_addr;
    struct addrinfo * ai_next
};
*/

//addrinfo, we don't really need it I guess


/*
struct sockaddr {
               sa_family_t sa_family;
               char        sa_data[14];
           }
*/

// the actual structure passed for the addr argument will depend on the address family.
// the only purpose of this structure is to cast the structure pointer passed in addr to avoid compiler warning
// so which base structure to chose?


/*
 * in guide, he casts sockaddr_storage to stock_addr
 * */

//use SOCK_STREAM
//reliable, two-way.
//out of band data transmission mechanism may be supported
//
//listen: which backlog to use, most application use 128
//
#define PORT "5000"

void	test_webserv(){
	
	fd_set master, read_fs, write_fs;
	int range;
	int	s_listen, s_new;
	struct sockaddr_storage clientaddr;
	struct sockaddr_storage serveraddr;
	socklen_t	addrlen;
	int			enable =1;

	/*test getaddrinfo */
	struct addrinfo hints, *returned, *current;

	hints.ai_family=AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//get server info
	if (getaddrinfo(NULL, PORT, &hints, &returned) != 0)
		throw(ConfigParser::parseError("getaddrinfo: ", strerror(errno)));

	//iter through returned addrinfo structures
	for (current = returned; current != NULL; current=current->ai_next) {

		s_listen = socket(AF_INET, SOCK_STREAM, 0);
		if (s_listen < 0) {
			std::cout<<"listen<0 skip"<<std::endl;
			continue ;
			//throw(ConfigParser::parseError("listen: ", strerror(errno)));
		}

		//SOL_SOCKET api level
		if (setsockopt(s_listen, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
			//throw(ConfigParser::parseError("setsockopt: ", strerror(errno)));
			std::cout<<"setopt<0 skip"<<std::endl;
			continue ;
		}

		std::cout<<"bind with current->ai_addr->sa_family  "<<current->ai_addr->sa_family<<std::endl;
		if (bind(s_listen, current->ai_addr, current->ai_addrlen) < 0 ) {
			//throw(ConfigParser::parseError("setsockopt: ", strerror(errno)));
			std::cout<<"bind<0 skip"<<std::endl;
			continue ;
		}
		break ; //valid addr
	}
	if (current == NULL){ // went through the whole thing without finding a valid addr
		throw(ConfigParser::parseError("getserver info, no valid addr returned: ", strerror(errno)));
	}
	freeaddrinfo(returned);

	//set listener  to non_blocking
	if (listen(s_listen, 128) == -1)
		throw(ConfigParser::parseError("listen: ", strerror(errno)));

	if (fcntl(s_listen, F_SETFL, O_NONBLOCK) == -1)
		throw(ConfigParser::parseError("fcntl: ", strerror(errno)));
	FD_SET(s_listen, &master);
	range = s_listen; //fd_max
	struct timeval tv;
	char buf[1000];
	memset(buf, 0, sizeof(buf));
	for (;;) {
		read_fs = master;
		write_fs = master;
		tv.tv_sec = 1; //on linux, tv is modified so need to reset for every call
		tv.tv_usec = 500000;
		if (select(range + 1, &read_fs, &write_fs, NULL, &tv) == -1)
			throw(ConfigParser::parseError("select: ", strerror(errno)));
		std::cout<<"listening..."<<std::endl;
		for (int i =0; i <= range;i++) {
			if (FD_ISSET(i, &read_fs)) {
				std::cout<<"found read connection fd: "<<i<<std::endl;
				if (i == s_listen) {
					std::cout<<"new connection" <<std::endl;
					addrlen = sizeof(clientaddr);
					s_new = accept(s_listen, reinterpret_cast<struct sockaddr *>(&clientaddr), &addrlen);
					if (s_new == -1) {
						std::cout<<"accept: "<<strerror(errno)<<std::endl;
					}
					else {
						std::cout<<"created client socket " <<s_new<<std::endl;
						FD_SET(s_new, &master);
						if (s_new > range) {
							range = s_new;
							/*print log here*/
						}
					}
				}
				else {
					int nbytes=recv(i, buf, sizeof buf, 0);
					if (nbytes <= 0){
						if (nbytes == 0)
							std::cout<<"closing connection "<<i<<std::endl;
						else
							std::cout<<"recv: "<<strerror(errno)<<std::endl;
						close(i);
						FD_CLR(i, &master);
					}
					else {
						std::cout<<"---RECEIVED---"<<std::endl;
						std::cout<<buf<<std::endl;
						memset(buf, 0, sizeof(buf));
					}
				}
			}
			else if (FD_ISSET(i, &write_fs))
			{
				//insert ReadRequest(i)
				std::cout<<"found write connection fd: "<<i<<std::endl;
				write(i, "OK", 2);
			}
		}
	}


	FD_ZERO(&master);
	FD_ZERO(&read_fs);

	//AF_INET ipv4


	close(s_listen);

	(void) master; (void)read_fs;
	(void)range;
	(void)s_listen; (void)s_new;
	(void)clientaddr;
	(void)serveraddr;
	(void)addrlen;
}

//from bind man
  //int sfd, cfd;
  //         struct sockaddr_un my_addr, peer_addr;
  //         socklen_t peer_addr_size;

  //         sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  //         if (sfd == -1)
  //             handle_error("socket");

  //         memset(&my_addr, 0, sizeof(my_addr));
  //                             /* Clear structure */
  //         my_addr.sun_family = AF_UNIX;
  //         strncpy(my_addr.sun_path, MY_SOCK_PATH,
  //                 sizeof(my_addr.sun_path) - 1);

  //         if (bind(sfd, (struct sockaddr *) &my_addr,
  //                 sizeof(my_addr)) == -1)
  //             handle_error("bind");

  //         if (listen(sfd, LISTEN_BACKLOG) == -1)
  //             handle_error("listen");

  //         /* Now we can accept incoming connections one
  //            at a time using accept(2) */

  //         peer_addr_size = sizeof(peer_addr);
  //         cfd = accept(sfd, (struct sockaddr *) &peer_addr,
  //                      &peer_addr_size);
  //         if (cfd == -1)
  //             handle_error("accept");

  //         /* Code to deal with incoming connection(s)... */

