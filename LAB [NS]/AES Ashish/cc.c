#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<openssl/aes.h>
#include<time.h>
#define PORT1 3004
#define IP "172.30.128.158"
//Hexadecimal key of 128 bits, As AES use 128 bits keys. This is symmetric key cryptography, so both server and client will have same symmetric key which is decided before.
static const unsigned char key[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
           };


// function to Decrypt the message received from receiver.
void aes_decry(char text [])
{

AES_KEY dkey;//decryption keys
unsigned char decout[256];

AES_set_decrypt_key(key, 128, &dkey);
//We have declared key as char array where above ,
// we need to set it to the format accptable by AES decyption function in openssl library.
//This particular function takes the char key and cenverts it into the AES_KEY format,
// which is used for decryption. so we have dkey as our decryption key.
AES_decrypt(text, decout, &dkey);
//A standard fucntion avaliable in openssl dev library that will take encrypted data received from
// the server which is in text here and decryppts it using the ket dkey gives output in the decout.
printf("\nServer : %s\n",decout);
if(strncmp(decout, "bye",3) == 0)
		{
			exit(1);
		}
}
// function to Encrypt the message from user.
void aes_encry(char text [], char * aa)
{

AES_KEY ekey; //Encryption keys

unsigned char out[256];
AES_set_encrypt_key(key, 128, &ekey);
//We have declared key as char array where above , we need to set it to the format accptable by AES encyption
//function in openssl library. This particular function takes the char key and cenverts it into the AES_KEY format,
//which is used for encryption. so we have ekey as our encryption key.
AES_encrypt(text, out, &ekey);
//System call to send the data accross the established network that is between client and server
//. sfd is over file descriptor for the socket, out is the array that contains the encryted text that we are sending,
//and next argument specifies the size of the data being sent.
strcpy(aa,out);

}


void main()
{
	int i;
	    unsigned char out[256];

    	unsigned char decout[256];

    	AES_KEY ekey,dkey;

	struct sockaddr_in serv_addr; // Structures for handling internet addresses

	unsigned char buf[256];

	bzero((char *) &serv_addr, sizeof(serv_addr));

  	serv_addr.sin_family = AF_INET;
// The variable serv_addr is a structure of type struct sockaddr_in. This structure has four fields. The first field is short sin_family, which contains a code for the address family. It should always be set to the symbolic constant AF_INET.

  	serv_addr.sin_port = htons(3004);

  	serv_addr.sin_addr.s_addr = INADDR_ANY;
// The second field of serv_addr is unsigned short sin_port , which contain the port number. However, instead of simply copying the port number to this field, it is necessary to convert this to network byte order using the function htons() which converts a port number in host byte order to a port number in network byte order.

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
// The socket() system call creates a new socket. It takes three arguments. The first is the address domain of the socket. The second argument is the type of socket, The third argument is the protocol.


	if(connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("error\n" );
		exit(0);
	}
	//The connect function is called by the client to establish a connection to the server. It takes three arguments, the socket file descriptor, the address of the host to which it wants to connect (including the port number), and the size of this address.

	do
	{

		memset(buf,0,255);
		printf("Client : ");
    		fgets(buf,255,stdin);

        	aes_encry(buf,&out);
		// calling AES_Encryption fuction , which will encrpt using openssl AES function
    		send(sfd,out,sizeof(out),0);

    		if(strncmp(buf, "bye",3) == 0)
		{
			exit(1);
		}
    		memset(buf,0,255);

		recv(sfd,buf,sizeof(buf),0);



		printf("\nEncrypted text received :");
		for(i =0;i<strlen(buf);i++)
		{
			printf("%d",buf[i]);
		}
// calling AES_Decryption fuction , which will decypt using openssl AES function
aes_decry(buf);

	}while(strncmp(decout,"bye",3)!=0);
}
