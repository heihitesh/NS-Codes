#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <openssl/des.h>
#include <openssl/rand.h>
#define MAX 4096 //max amt of bits
#define PORT 8081



void func(int sockfd)
{

   DES_cblock key;

   DES_cblock seed = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
   DES_cblock ivsetup = {0xE1, 0xE2, 0xE3, 0xD4, 0xD5, 0xC6, 0xC7, 0xA8};
   DES_key_schedule keysched;

   RAND_seed(seed, sizeof(DES_cblock));
   DES_string_to_key("HITESH",&key);
   DES_set_key((DES_cblock *)key, &keysched);

    char buff[MAX];
  	char out[MAX];
  	int n,len;
  	// infinite loop for chat
  	for (;;) {
		bzero(buff, MAX);
    bzero(out, MAX);
		// read the message from client and copy it in buffer
		read(sockfd, out, sizeof(out));

    // DES_ecb_encrypt((DES_cblock *)out,(DES_cblock *)buff, &keysched, DES_DECRYPT);
    len = strlen(out);
    DES_ncbc_encrypt((unsigned char*) out,(unsigned char*) buff, len, &keysched, &ivsetup, DES_DECRYPT);

		// print buffer which contains the client contents
		printf("C : %s : %s",out, buff);
    printf("S - ");
		bzero(buff, MAX);
		bzero(out, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n');

		// and send that buffer to client
    //  DES_ecb_encrypt((DES_cblock *)buff,(DES_cblock *)out, &keysched, DES_ENCRYPT);

      len = strlen(buff);

      DES_ncbc_encrypt((unsigned char*) buff,(unsigned char*) out, len, &keysched, &ivsetup, DES_ENCRYPT);
  //it takes unsigned char input // (input,output,length)

    printf("Encypted Msg : %s", out);
		write(sockfd, out, sizeof(out));

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
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
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
