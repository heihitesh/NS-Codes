#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <openssl/des.h>
#include <openssl/rand.h>
#define MAX 80 //specific size of buffer
#define PORT 8080 //number of port


// Function designed for chat between client and server.
void func(int sockfd)
{
DES_cblock Key1 = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
DES_cblock Key2 = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
DES_cblock Key3 = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
/* in the file des.h some fixed values for seed, to make encryption and decryption are matched ,
seed should br random but it depend of the PRNG(Pseudo Random Number Generator that it is deterministic)*/
DES_cblock seed = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
//The key schedule is an expanded form of the key; it is used to speed the encryption process
DES_key_schedule ksh1,ksh2,ksh3;
/*RAND_seed (const void *buf, int num); mixes the num bytes at buf into the PRNG state, here num=entropy
that is an estimate of how much randomness is contained in buf*/
RAND_seed(seed, sizeof(DES_cblock));

// DES_set_key:Setup a des key schedule from a key
DES_set_key((DES_cblock *)Key1, &ksh1);
DES_set_key((DES_cblock *)Key2, &ksh2);
DES_set_key((DES_cblock *)Key3, &ksh3);
	char buff[MAX];
	char out[MAX];
	int n;
	// infinite loop for chat
	for (;;) {
        bzero(buff, MAX);
        bzero(out, MAX);
		// read the message from client and copy it in buffer "out"
		read(sockfd, out, sizeof(out));



/*Encrypt/decrypt a block using DE, here using ECB mode
DES_ecb_encrypt function
Parameters:
the first argument :input data to encrypt
the second argumentoutput: data after encryption
the third argumentoutput: key schedule to use
the forth argumentoutput:encp argument if non zero, encrypt. if zero, decrypt. */
        printf("From client before decyption: %s\t To client : ", out);
        DES_ede3_cbc_encrypt((DES_cblock *)out,(DES_cblock *)buff,sizeof(buff),&ksh1, &ksh2, &ksh3,&seed, DES_DECRYPT);
		// print buffer which contains the client contents
		printf("\n From client after decyption: %s\t To client : ", buff);
		bzero(buff, MAX);
		bzero(out, MAX);
		n = 0;
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
         DES_ede3_cbc_encrypt((DES_cblock *)buff,(DES_cblock *)out,sizeof(out),&ksh1, &ksh2, &ksh3,&seed, DES_ENCRYPT);
          printf("\n Encypted Msg : %s", out);
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
/*1.	Create a socket with the socket() system call
2.	Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists of a port number on the host machine.
3.	Listen for connections with the listen() system call
4.	Accept a connection with the accept() system call. This call typically blocks until a client connects with the server.
5.	Send and receive data
*/


//int sockfd, connfd: These two variables store the values returned by the socket system call and the accept system call.
//Len:  stores the size of the address of the client. This is needed for the accept system call.
	int sockfd, connfd, len;

	/*struct sockaddr_in servaddr, cli: A sockaddr_in is a structure containing an internet address. This structure is defined in <netinet/in.h>.
	*/
	struct sockaddr_in servaddr, cli;

	// socket create and verification

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
	/*the function bzero() sets all values in a buffer to zero.
	It takes two arguments, the first is a pointer to the buffer and the
	second is the size of the buffer. Thus, this line initializes serv_addr to zeros.

*/
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	/*: The variable serv_addr is a structure of type struct sockaddr_in.
	This structure has four fields. The first field is short sin_family,
	which contains a code for the address family. It should always be set to the symbolic constant AF_INET.*/
	servaddr.sin_family = AF_INET;

	/*:The third field of sockaddr_in is a structure of type struct in_addr which contains only a single field unsigned long s_addr.
	This field contains the IP address of the host. For server code, this will always be the IP address of
	the machine on which the server is running, and there is a symbolic constant INADDR_ANY which gets this address.*/
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//:The second field of serv_addr is unsigned short sin_port , which contain the port number.
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	//The bind() system call binds a socket to an address.
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	/*The listen system call allows the process to listen on the socket for connections.
	The first argument is the socket file descriptor, and the second is the size of the backlog queue .*/
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	//The accept() system call causes the process to block until a client connects to the server.
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
