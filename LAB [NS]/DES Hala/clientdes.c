
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <openssl/des.h>
#include <openssl/rand.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

void func(int sockfd) 
{ 
//DES_cblock type is consists of 8 bytes with odd parity. The least significant bit in each byte is the parity bit
DES_cblock key;
/* in the file des.h some fixed values for seed, to make encryption and decryption are matched ,
seed should br random but it depend of the PRNG(Pseudo Random Number Generator that it is deterministic)*/
DES_cblock seed = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
//The key schedule is an expanded form of the key; it is used to speed the encryption process
DES_key_schedule keysched;
/*RAND_seed (const void *buf, int num); mixes the num bytes at buf into the PRNG state, here num=entropy 
that is an estimate of how much randomness is contained in buf*/
RAND_seed(seed, sizeof(DES_cblock));
// DES_string_to_key:Convert a string to a DES key. Use something like PKCS5_PBKDF2_HMAC_SHA1() to create key from passwords. 
DES_string_to_key("hala",&key);
// DES_set_key:Setup a des key schedule from a key
DES_set_key((C_Block *)key, &keysched);

	char buff[MAX]; 
	char out[MAX];
	int n; 
	// infinite loop for chat 
	for (;;) { 
		bzero(buff, sizeof(buff)); 
		bzero(out, sizeof(buff)); 
		printf("Enter the string : "); 
		n = 0; 
		while ((buff[n++] = getchar()) != '\n') 
			; 
		
/*Encrypt/decrypt a block using DE, here using ECB mode
DES_ecb_encrypt function
Parameters:
the first argument :input data to encrypt
the second argumentoutput: data after encryption
the third argumentoutput: key schedule to use
the forth argumentoutput:encp argument if non zero, encrypt. if zero, decrypt. */
   DES_ecb_encrypt((C_Block *)buff,(C_Block *)out, &keysched, DES_ENCRYPT);
        printf("\n Encypted Msg : %s", out);
		// and send that buffer to server
		write(sockfd, out, sizeof(out)); 
		bzero(buff, sizeof(buff)); 
		bzero(buff, sizeof(out)); 
         // read the message from server and copy it in Buffer "out"         
		read(sockfd, out, sizeof(out)); 
		//decrypt the content of buffer"out"
        DES_ecb_encrypt((C_Block *)out,(C_Block *)buff, &keysched, DES_DECRYPT);
		printf("From Server after decryption : %s", buff); 
		// if msg contains "Exit" then server exit and chat ended. 
		if ((strncmp(buff, "exit", 4)) == 0) { 
			printf("Client Exit...\n"); 
			break; 
		} 
	} 
} 

int main() 
{ 
/*The steps involved in establishing a socket on the client side are as follows:
1.	Create a socket with the socket() system call
2.	Connect the socket to the address of the server using the connect() system call
3.	Send and receive data. There are a number of ways to do this, but the simplest is to use the read() and write() system calls
*/


//int sockfd, connfd: These two variables store the values returned by the socket system call and the accept system call.

	int sockfd, connfd; 
	/*struct sockaddr_in servaddr, cli: A sockaddr_in is a structure containing an internet address. This structure is defined in <netinet/in.h>.
	*/
	struct sockaddr_in servaddr, cli; 

	// socket create and varification 
	
/*The socket() system call creates a new socket. It takes three arguments. 
The first is the address domain of the socket(AF_INET :Internet domain), 
the second argument is the type of socket,  The third argument is the protocol.
 If this argument is zero the operating system will choose the most appropriate protocol.
 It will choose TCP for stream sockets and UDP for datagram sockets.*/
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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	/*The connect function is called by the client to establish a connection to the server. 
	It takes three arguments, the socket file descriptor, the address of the host to which it
	wants to connect (including the port number), and the size of this address. This function returns 0 on success and -1 if it fails.
The client also use the same encrypt and decrypt functions that are used by the server . 
*/
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
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


