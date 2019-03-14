
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define PORT 33333

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
	serv_addr.sin_port=htons(PORT);

	connect(cli_skt,(struct sockaddr *)&serv_addr,sizeof(serv_addr));  /*called by the client to establish a connection to the server. first arg -the socket file descriptor, second arg- the address of the host to which it wants to connect (including the port number), third arg-the size of this address*/

	printf("\n\nConnection Accepted From Server, Server IP:%s , Port:%d",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));


	while(1)
	{	printf("\n");

		memset(msg,0,MAX);                   //fill a block of memory with a particular value- initialize buffer
		printf("Client :");
		gets(msg);

              	for(i = 0; msg[i] != '\0'; ++i){     //encryption using key k
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




		send(cli_skt,msg,strlen(msg),0);               //initiate transmission of a message from the specified socket to its peer



		memset(msg,0,MAX);
		recv(cli_skt,msg,MAX,0);
                 if(strcmp(msg,"bye")==0)                      //condition to terminate
		{


			printf("\n\nConnection Closed\n\n");
                break;
		}
               for(i = 0; msg[i] != '\0'; ++i){               //on receiving reply- decryption process using key
		ch = msg[i];

		if(ch >= 'a' && ch <= 'z'){
			ch = ch - key;

			if(ch < 'a'){
				ch = ch + 'z' - 'a' + 1;
			}

			msg[i] = ch;
		}
		else if(ch >= 'A' && ch <= 'Z'){
			ch = ch - key;

			if(ch < 'A'){
				ch = ch + 'Z' - 'A' + 1;
			}

			msg[i] = ch;
		}
	}
		printf("Server :%s",msg);

	}
	close(cli_skt);                                           //close client connection
	return 0;
}
