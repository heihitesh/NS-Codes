#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> //The header file netinet/in.h contains constants and structures needed for internet domain addresses. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PORT1 43454
#define PORT2 43453
#define MAX 100
#define MAX_CLI 1

char msg[MAX];


int main()
{

int cli_skt,key=5,i=0;
        char ch;
	struct sockaddr_in serv_addr;                  //A sockaddr_in is a structure containing an internet address.
	cli_skt=socket(PF_INET,SOCK_STREAM,0);         //socket() system call creates a new socket.First arg-domain address, second arg- 						               //type of socket, third arg- to choose protocol - TCP for stream sockets 
	printf("\n\nClient Socket Created:%d",cli_skt);
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(PORT1);

	connect(cli_skt,(struct sockaddr *)&serv_addr,sizeof(serv_addr));  /*called by the client to establish a connection to the server. first arg -the socket file descriptor, second arg- the address of the host to which it wants to connect (including the port number), third arg-the size of this address*/
	
	printf("\n\nConnection Accepted From Server, Server IP:%s , Port:%d",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
	

  int serv_skt1,cli_skt1,cli_len1,i1=0,key1=5;
        char ch1;
	struct sockaddr_in serv_addr1,cli_addr1;    //A sockaddr_in is a structure containing an internet address.
	serv_skt1=socket(PF_INET,SOCK_STREAM,0);   //socket() system call creates a new socket.First arg-domain address, second arg-type of   							  //socket, third arg- to choose protocol - TCP for stream sockets 
	printf("\n\nServer Socket Created:%d",serv_skt1);
	
	serv_addr1.sin_family=AF_INET;             // code for the address family
	serv_addr1.sin_addr.s_addr=htonl(INADDR_ANY);  //contains the IP address of the host
	serv_addr1.sin_port=htons(PORT2);           //contain the port number

	bind(serv_skt1,(struct sockaddr *)&serv_addr1,sizeof(serv_addr1));   //binds a socket to an address
	printf("\n\nBind Successful, Server IP:%s , Port:%d",inet_ntoa(serv_addr1.sin_addr),ntohs(serv_addr1.sin_port));
		
	printf("\n\nWaiting For Client Connection");

	listen(serv_skt1,MAX_CLI);                 //allows the process to listen on the socket for connections
						  //First arg- socket file descriptor,second arg-the number of connections that can be waiting
	
	

	cli_len1=sizeof(cli_addr1);
	cli_skt1=accept(serv_skt1,(struct sockaddr *)&cli_addr1,&cli_len1);    //causes the process to block until a client connects to the server
	
	printf("\n\nConnection Accepted From Client, IP:%s , Port:%d",inet_ntoa(cli_addr1.sin_addr),ntohs(cli_addr1.sin_port));
	while(1)
	{	
		printf("\n");

		memset(msg,0,MAX);                //fill a block of memory with a particular value- initialize buffer
		recv(cli_skt1,msg,MAX,0);          //block until there is something for it to read in the socket

		  
                
                if(strcmp(msg,"bye")==0)         //condition to exit
		{
                         	
		           
			printf("\n\nConnection Closed\n\n");
                        send(cli_skt,"bye",strlen("bye"),0);
	                			break;
		}
                   
		printf("Client :%s",msg);
                send(cli_skt,msg,strlen(msg),0);
              
		memset(msg,0,MAX);
                recv(cli_skt,msg,MAX,0);   
		printf("\n");
		printf("Server :");
		
	        printf(" message: %s", msg);
		send(cli_skt1,msg,strlen(msg),0);	//initiate transmission of a message from the specified socket to its peer.

	}
	close(cli_skt);                                            //terminate client socket
		                                   //terminate server socket
  
	close(cli_skt1);                                            //terminate client socket
	close(serv_skt1);
         

      

	
		return 0;
}

