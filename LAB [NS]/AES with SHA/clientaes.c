
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#define MAX 256
#define PORT 8080

//Hexadecimal key of 128 bits, As AES use 128 bits keys.
//This is symmetric key cryptography, so both server and client will have same symmetric key which is decided before.
static const unsigned char key[] =
{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};


//
//* AES key for Encryption and Decryption */
//static const unsigned char key[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; //128




void func(int sockfd)
{
AES_KEY enc_key, dec_key;
//AES-128 bit CBC Encryption
AES_set_encrypt_key(key, 128, &enc_key);
//We have declared key as char array where above ,
// we need to set it to the format accptable by AES decyption/encyption function in openssl library.
//This particular function takes the char key and cenverts it into the AES_KEY format,
// which is used for decryption. so we have dkey as our decryption key.



/* AES-128 bit CBC Decryption */
AES_set_decrypt_key(key,128,&dec_key);

	char text[MAX];
	char enc[MAX];
	char dec[MAX];
	int n;
	// infinite loop for chat
	for (;;) {
		bzero(text, sizeof(text));
		bzero(enc, sizeof(enc));
		printf("Enter the string : ");
		n = 0;
		while ((text[n++] = getchar()) != '\n');

    AES_encrypt(text,enc, &enc_key);
    printf("\n Encypted Msg : %s", enc);

    // unsigned char outHash[30];
    //
    //
    // SHA1(text, strlen(text), outHash);
    // /*s a cryptographic hash function with a 160 bit output.
    // SHA1() computes the SHA-1 message digest of the n bytes at d and places it in md (which must have space for SHA_DIGEST_LENGTH == 20 bytes of output).
    //  If md is NULL, the digest is placed in a static array. Note: setting md to NULL is not thread safe.
    // */
    //
    //
    // printf("Hashed msg : %s", concat(&text,&outHash));


  //  char data[] = "Hello, world!";
    int length = strlen(text);
    unsigned char hash[30];
    unsigned char comma[1] = {','};

    SHA1(text, length, hash);

    char *result = malloc(strlen(enc) + strlen(hash) + strlen(comma) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, enc);
    strcat(result, comma);
    strcat(result, hash);

    printf("\n Enc Hash %s .END\n", result );


		// and send that buffer to server
		write(sockfd, &enc, sizeof(enc));


		bzero(text, sizeof(text));
		bzero(enc, sizeof(enc));
		bzero(dec, sizeof(dec));
    // read the message from server and copy it in Buffer "text"
		read(sockfd, text, sizeof(text));
    AES_decrypt(text, dec, &dec_key);
		printf("From Server after decryption : %s",dec);
		// if msg contains "Exit" then server exit and chat ended.
		if ((strncmp(dec, "exit", 4)) == 0) {
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
