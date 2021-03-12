#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include <unistd.h>

#include<sys/socket.h>

#include<netinet/in.h>

#include<netinet/ip.h>

#include<string.h>

#include<arpa/inet.h>

#include<sys/select.h>
#include <iomanip>
#include<iostream>





int main(void) {

        int b1, b2, l1;

        int on = 1;

        struct sockaddr_in ars, tech, tech3;

        struct in_addr nica;



        memset(&ars, 0, sizeof(ars));

        ars.sin_family = AF_INET;

        ars.sin_addr.s_addr = htonl(INADDR_ANY);

        ars.sin_port = htons(12345);



        memcpy(&tech, &ars, sizeof(ars));

        inet_pton(AF_INET, "127.0.0.1", &nica);

        tech.sin_addr.s_addr = nica.s_addr;

        tech.sin_port = htons(12345);

        memcpy(&tech3, &ars, sizeof(ars));

        inet_pton(AF_INET, "127.0.0.2", &nica);

        tech3.sin_addr.s_addr = nica.s_addr;

        tech3.sin_port = htons(12345);




		int	max = 0;

        b1 = socket(AF_INET, SOCK_STREAM, 0);
		if (b1 > max)
			max = b1;


        b2 = socket(AF_INET, SOCK_STREAM, 0);
		if (b2 > max)
			max = b2;

        l1 = socket(AF_INET, SOCK_STREAM, 0);
		if (l1 > max)
			max = l1;



        if (setsockopt(b1, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) {

                perror("setsockopt b1:");

                return 1;

        }

        if (bind(b1, (struct sockaddr*)&tech, sizeof(tech))){

                perror("bind b1:");

                return 1;

        }

       // if (connect(b1, (struct sockaddr *)&tech, sizeof(tech))){

       //         perror("connect b1:");

       //         return 1;

       // }



        if (setsockopt(b2, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) {

                perror("setsockopt b2:");

                return 1;

        }

        if (bind(b2, (struct sockaddr*)&tech3, sizeof(tech3))){

                perror("bind b2:");

                return 1;

        }

        tech.sin_port = htons(12346);

       // if (connect(b2, (struct sockaddr *)&tech, sizeof(tech))){

       //         perror("connect b2:");

       //         return 1;

       // }



        if (setsockopt(l1, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) {

                perror("setsockopt l1:");

                return 1;

        }

        if (bind(l1, (struct sockaddr*)&ars, sizeof(ars))){

                perror("bind l1:");

                return 1;

        }

        if (listen(l1, 10)){

                perror("listen:");

                return 1;

        }

        if (listen(b2, 11)){

                perror("listen:");

                return 1;

        }






 			fd_set rfds;
 			fd_set master;
           struct timeval tv;
           int retval;

           /* Watch stdin (fd 0) to see when it has input. */

           FD_ZERO(&rfds);
           FD_ZERO(&master);
           FD_SET(l1, &master);
           FD_SET(b2, &master);


		   max = 0;
		   if (l1 > max)
			   max = l1;
		   if (b2 > max)
			   max = b2;

           /* Wait up to five seconds. */

           tv.tv_sec = 20;
           tv.tv_usec = 0;

           retval = select(max + 1, &master, NULL, NULL, &tv);
           /* Don't rely on the value of tv now! */

           if (retval == -1)
               perror("select()");
           else if (retval)
		   {
               printf("Data is available now.\n");
               if (FD_ISSET(l1, &master))
			   {
				   std::cout<<"l1!"<<std::endl;
				   std::cout<<inet_ntoa(tech.sin_addr)<<std::endl;

			   }
               if (FD_ISSET(b1, &master))
			   {
				   std::cout<<"b1!"<<std::endl;
				   std::cout<<inet_ntoa(tech.sin_addr)<<std::endl;

			   }
			   if (FD_ISSET(b2, &master))
				   std::cout<<"b2!"<<std::endl;
		   }
           else
               printf("No data within five seconds.\n");
        	//accept(l1, 0, 0);
        return 0;
}



