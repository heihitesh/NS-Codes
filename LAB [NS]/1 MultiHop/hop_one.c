// Write CPP code here
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PORT1 43453
#define PORT2 43452
#define MAX 100
#define MAX_CLI 1


void func(int sockfd_c, int sockfd_s)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));

    //Node <-------->(Server) [THIS NODE] (Client) <-------->Node2
    read(sockfd_s, buff, sizeof(buff)); //reciving data from a server socket
                  //decrypt(buff);
    printf("From Server after decryption : %s", buff);
    if ((strncmp(buff, "exit", 4)) == 0) {  // first and second arg : string to compare // third arg : max no of char to comapre
      printf("Client Exit...\n");
      break;
    }

    write(sockfd_c,buff,sizeof(buff)); // sending data in a Client Socket
    bzero(buff, sizeof(buff));

    read(sockfd_c,buff,sizeof(buff)); // recivning data from client socket
    write(sockfd_s,buff,sizeof(buff));
	}
}

int main()
{
	int sockfd_c;
	struct sockaddr_in servaddr; ////A sockaddr_in is a structure containing an internet address.

	// socket create and varification
	sockfd_c = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_c == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr)); // set the buffer to zeros

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //it shall convert the string , in the standard IPv4 dotted decimal notation,
	servaddr.sin_port = htons(PORT1);  //The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.


	// connect the client socket to server socket
	if (connect(sockfd_c, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
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

    int sockfd_s;
  	struct sockaddr_in serv_addr1,cli_addr1;    //A sockaddr_in is a structure containing an internet address.
  	sockfd_s=socket(AF_INET,SOCK_STREAM,0);   //socket() system call creates a new socket.First arg-domain address, second arg-type of   							  //socket, third arg- to choose protocol - TCP for stream sockets
  	printf("\n\nServer Socket Created:%d",sockfd_s);

  	serv_addr1.sin_family=AF_INET;             // code for the address family
  	serv_addr1.sin_addr.s_addr=htonl(INADDR_ANY);  //contains the IP address of the host
  	serv_addr1.sin_port=htons(PORT2);           //contain the port number of the Main Client

  	bind(sockfd_s,(struct sockaddr *)&serv_addr1,sizeof(serv_addr1));   //binds a socket to an address
  	printf("\n\nBind Successful, Server IP:%s , Port:%d",inet_ntoa(serv_addr1.sin_addr),ntohs(serv_addr1.sin_port));

  	printf("\n\nWaiting For Client Connection");

  	listen(sockfd_s,5);                 //allows the process to listen on the socket for connections
  						  //First arg- socket file descriptor,second arg-the number of connections that can be waiting

  	int cli_len1 = sizeof(cli_addr1);
  	int cli_skt1= accept(sockfd_s,(struct sockaddr *)&cli_addr1,&cli_len1);    //causes the process to block until a client connects to the server

  	printf("\n\nConnection Accepted From Client, IP:%s , Port:%d",inet_ntoa(cli_addr1.sin_addr),ntohs(cli_addr1.sin_port));


	func(sockfd_c,cli_skt1); // 1 arg: client FD
                            // 2 arg : server FD

	// close the socket
  close(sockfd_s);
	close(sockfd_c);
}
