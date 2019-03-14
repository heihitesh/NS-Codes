#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>	
#include <fcntl.h>
#include <fstream>
#include <openssl/aes.h>	//to get aes from openssl library
using namespace std;
//Client side
//Get a 128 bit AES key for encryption and decryption (Should be same as Server)
static const unsigned char key[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
       0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
           }; 

int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0); 
    } //grab the IP address and port number 
    char *serverIp = argv[1]; int port = atoi(argv[2]); 
    //create a message buffer 
   unsigned char msg[256]; 

	unsigned char out[256]; 

    unsigned char decout[256];
	AES_KEY ekey,dkey;
    //setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; 
	return -1;
    }
    cout << "Connected to the server!" << endl;
   
    while(1)
    {
        memset(&msg, 0, sizeof(msg));
        cout << ">";
        cin>>msg;
      
	//AES Encryption works here with key defined
	AES_set_encrypt_key(key, 128, &ekey);
	AES_encrypt(msg, out, &ekey); 
        if(!strcmp((char *)msg,"exit"))
        {
            send(clientSd, (char*)&out, strlen((char *)out), 0);
            break;
        }
	//Send encrypted message to server
        send(clientSd, (char*)&out, strlen((char *)out), 0);
        cout << "Awaiting server response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
	//Receive encrypted message from server
        recv(clientSd, (char*)&msg, sizeof(msg), 0);
	//AES Decryption goes here using same key
	AES_set_decrypt_key(key, 128, &dkey);
	AES_decrypt(msg, decout, &dkey);
        if(!strcmp((char *)decout, "exit"))
        {
            cout << "Server has quit the session" << endl;
            break;
        }
        cout << "Encrypted (Received): " ;
	for(int i =0;i<strlen((char *)msg);i++)
		{
			printf("%d",msg[i]);
		}
	cout<<endl;
	
	 cout << "Server : " << decout<< endl;
    }
   
    close(clientSd);
  
    cout << "Connection closed" << endl;
    return 0;    
}

