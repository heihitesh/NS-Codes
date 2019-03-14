// Write CPP code here
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 80
#define PORT 43452

//client -> hop1 ->hop2 ->server


void func(int sockfd)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');  //note '\n' is stored in the buff at the end then it terminate
                 // encrypt(buff);
                 printf("Encypted Msg : %s", buff);
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));

		read(sockfd, buff, sizeof(buff));
                  //decrypt(buff);
		printf("From Server after decryption : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {  // first and second arg : string to compare // third arg : max no of char to comapre
			printf("Client Exit...\n");
			break;
		}
	}
}

int main()
{
	int sockfd;
	struct sockaddr_in servaddr; ////A sockaddr_in is a structure containing an internet address.

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr)); // set the buffer to zeros

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //it shall convert the string , in the standard IPv4 dotted decimal notation,
	servaddr.sin_port = htons(PORT);  //The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.


	// connect the client socket to server socket
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
  	 /*called by the client to establish a connection to the server.
  	 first arg -the socket file descriptor,
     second arg- the address of the host/server to which it wants to connect (including the port number),
  	 third arg-the size of this address*/
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
