#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> //The header file netinet/in.h contains constants and structures needed for internet domain addresses.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PORT 33333
#define MAX 100
#define MAX_CLI 1

char msg[MAX];

int main()
{
	int serv_skt,cli_skt,cli_len,i=0,key=5;
        char ch;
	struct sockaddr_in serv_addr,cli_addr;    //A sockaddr_in is a structure containing an internet address.
	serv_skt=socket(PF_INET,SOCK_STREAM,0);   //socket() system call creates a new socket.First arg-domain address, second arg-type of   							  //socket, third arg- to choose protocol - TCP for stream sockets
	printf("\n\nServer Socket Created:%d",serv_skt);

	serv_addr.sin_family=AF_INET;             // code for the address family
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);  //contains the IP address of the host
	serv_addr.sin_port=htons(PORT);           //contain the port number

	bind(serv_skt,(struct sockaddr *)&serv_addr,sizeof(serv_addr));   //binds a socket to an address
	printf("\n\nBind Successful, Server IP:%s , Port:%d",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));

	printf("\n\nWaiting For Client Connection");

	listen(serv_skt,MAX_CLI);                 //allows the process to listen on the socket for connections
						  //First arg- socket file descriptor,second arg-the number of connections that can be waiting



	cli_len=sizeof(cli_addr);
	cli_skt=accept(serv_skt,(struct sockaddr *)&cli_addr,&cli_len);    //causes the process to block until a client connects to the server

	printf("\n\nConnection Accepted From Client, IP:%s , Port:%d",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));



	printf("\n");
	while(1)
	{
		printf("\n");

		memset(msg,0,MAX);                //fill a block of memory with a particular value- initialize buffer
		recv(cli_skt,msg,MAX,0);          //block until there is something for it to read in the socket


                 for(i = 0; msg[i] != '\0'; ++i){
		ch = msg[i];

		if(ch >= 'a' && ch <= 'z'){
			ch = ch - key;             //shifting of alphabets using key- decryption (lower case)

			if(ch < 'a'){
				ch = ch + 'z' - 'a' + 1;
			}

			msg[i] = ch;
		}
		else if(ch >= 'A' && ch <= 'Z'){
			ch = ch - key;             //shifting of alphabets using key- decryption (upper case)

			if(ch < 'A'){
				ch = ch + 'Z' - 'A' + 1;
			}

			msg[i] = ch;
		}
	}
                if(strcmp(msg,"bye")==0)         //condition to exit
		{


			printf("\n\nConnection Closed\n\n");
                       send(cli_skt,"bye",strlen("bye"),0);
	                			break;
		}

		printf("Client :%s",msg);


		memset(msg,0,MAX);
		printf("\n");
		printf("Server :");
		gets(msg);
         for(i = 0; msg[i] != '\0'; ++i){                         //encryption of reply of server
		ch = msg[i];

		if(ch >= 'a' && ch <= 'z'){
			ch = ch + key;

			if(ch > 'z'){
				ch = ch - 'z' + 'a' - 1;
			}

			msg[i] = ch;
		}
		else if(ch >= 'A' && ch <= 'Z'){
			ch = ch + key;

			if(ch > 'Z'){
				ch = ch - 'Z' + 'A' - 1;
			}

			msg[i] = ch;
		}
	}

	printf("Encrypted message: %s", msg);
		send(cli_skt,msg,strlen(msg),0);	//initiate transmission of a message from the specified socket to its peer.

	}
	close(cli_skt);                                            //terminate client socket
	close(serv_skt);	                                   //terminate server socket
	return 0;
}
