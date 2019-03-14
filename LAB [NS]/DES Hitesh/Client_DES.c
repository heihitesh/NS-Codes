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
#define MAX 4096
#define PORT 8081


void func(int sockfd)
{

// A DES key is of type DES_cblock which consists of 8 bytes with odd parity.
// The least significant bit in each byte is the parity bit
DES_cblock key;
//DES_cblock ivec;
DES_cblock seed = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10}; //64bit seed OctalRepresentation
//For the first block we start with an initiallization vector (ivec).
DES_cblock ivsetup = {0xE1, 0xE2, 0xE3, 0xD4, 0xD5, 0xC6, 0xC7, 0xA8};

//The key schedule is an expanded platform-dependent form of the key which is used to speed the encryption process.
//The first is the generation of a DES_key_schedule from a key
DES_key_schedule keysched;

//RAND_seed()) to generate a random 64-bit DES key
RAND_seed(seed, sizeof(DES_cblock));

DES_string_to_key("HITESH",&key);

//will check that the key passed is of odd parity and is not a weak or semi-weak key.
DES_set_key((DES_cblock *)key, &keysched); // set the DES key to the keyschedule

	  char buff[MAX];
	  char out[MAX];
	  int n,len;
	  for (;;) {
		//The bzero() function shall place n zero-valued bytes in the area pointed to by s.
		bzero(buff, sizeof(buff));
		bzero(out, sizeof(buff));
		printf("C - ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n'); // wait until we press enter

		//If the encrypt argument is DES_ENCRYPT,
		// the input (plaintext) is encrypted into the output (ciphertext) using the specified key_schedule
		//  DES_ecb_encrypt((DES_cblock *)buff,(DES_cblock *)out, &keysched, DES_ENCRYPT);
		len = strlen(buff);

		//DES_ncbc_encrypt() encrypts/decrypts using the cipher-block-chaining (CBC) mode of DES
		DES_ncbc_encrypt((unsigned char*) buff,(unsigned char*) out, len, &keysched, &ivsetup, DES_ENCRYPT);
		//it takes unsigned char input // (input,output,length)

    printf("S : %s", out);
		write(sockfd, out, sizeof(out));

		//Clearing the Buffer
		bzero(buff, sizeof(buff));
		bzero(buff, sizeof(out));

		read(sockfd, out, sizeof(out)); // all the incomming data is stored in out
		len = strlen(out);
		// If the encrypt argument is DES_DECRYPT, the input (ciphertext) is decrypted into the output (plaintext).
  	DES_ncbc_encrypt((unsigned char*) out,(unsigned char*) buff, len, &keysched, &ivsetup, DES_DECRYPT);
		//it takes unsigned char input // (input,output,length)
		//Therfore the result is stored in buff


		printf("D : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("\n<---------------Client Exit-------------->\n");
			break;
		}
	}
}

int main()
{
	int sockfd;
	struct sockaddr_in servaddr;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	  bzero(&servaddr, sizeof(servaddr));
		//The bzero() function shall place n zero-valued bytes in the area pointed to by s.
		//because we are using a

		// assign IP, PORT
	   servaddr.sin_family = AF_INET;
	   servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	   servaddr.sin_port = htons(PORT);

	   // connect the client socket to server socket
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
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
