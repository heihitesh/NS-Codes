#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 80
#define PORT 8080




void encrypt(char *buff)
{int key=3;
  int i;
  char ch;
  for(i = 0; buff[i] != '\0'; ++i){
  ch = buff[i];

  if(ch >= 'a' && ch <= 'z'){
    ch = ch + key;

    if(ch > 'z'){
      ch = ch - 'z' + 'a' - 1;
    }

    buff[i] = ch;
  }
  else if(ch >= 'A' && ch <= 'Z'){
    ch = ch + key;

    if(ch > 'Z'){
      ch = ch - 'Z' + 'A' - 1;
    }

    buff[i] = ch;
  }
}
}

void decrypt(char *buff)
{int key=3;
  int i;
  char ch;
for(i = 0; buff[i] != '\0'; ++i){
  ch = buff[i];

  if(ch >= 'a' && ch <= 'z'){
    ch = ch - key;

    if(ch < 'a'){
      ch = ch + 'z' - 'a' + 1;
    }

    buff[i] = ch;
  }
  else if(ch >= 'A' && ch <= 'Z'){
    ch = ch - key;

    if(ch < 'A'){
      ch = ch + 'Z' - 'A' + 1;
    }

    buff[i] = ch;
  }
}

}
// Function designed for chat between client and server.
void func(int sockfd)
{
	char buff[MAX];
	int n;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
                 decrypt(buff);
		// print buffer which contains the client contents
		printf("From client after decyption: %s\t To client : ", buff);
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
                  encrypt(buff);
                   printf("Encypted Msg : %s", buff);
		write(sockfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;  //
	struct sockaddr_in servaddr, cli;  ////A sockaddr_in is a structure containing an internet address.

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
                                                  //this will always be the
                                                  //IP address of the machine on which the server is running, (localhost)
                                                  //and there is a symbolic constant INADDR_ANY which gets this address.
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
  //allows the process to listen on the socket for connections
						  //First arg- socket file descriptor,second arg-the number of connections that can be waiting

	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		printf("server acccept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}
