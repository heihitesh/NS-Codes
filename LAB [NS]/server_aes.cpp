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
#include <openssl/aes.h>			//to get aes from openssl library
using namespace std;
//Server side
//get a 128 bit AES key for encryption and decryption (Should be same as client)
static const unsigned char key[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
       0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
           };

int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //grab the port number
    int port = atoi(argv[1]);
    //buffer to send and receive messages with
   unsigned  char msg[256];
	
	unsigned char out[256]; //contains encrypted text that should be sent

    	unsigned char decout[256];//Contains decrypted text that is received
	
	AES_KEY ekey,dkey; 
     
    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
    cout << "Connected with client!" << endl;
   
    
    while(1)
    {
        //receive a message from the client (listen)
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
		//Received Encrypted msg from client
        recv(newSd, (char*)&msg, sizeof(msg), 0);
	        
        cout << "Encrypted (Received): " ;
		for(int i =0;i<strlen((char *)msg);i++)
		{
			printf("%d",msg[i]);
		}
       cout<<endl;
	//AES decryption with same decryption key
	AES_set_decrypt_key(key, 128, &dkey);
	AES_decrypt(msg, decout, &dkey);
	cout<<"Client: "<<decout<<endl;
	if(!strcmp((char *)decout, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
	memset(&msg, 0, sizeof(msg)); 
        cout << ">";
        cin>>msg;
   
			//AES Encryption works here with key defined
	AES_set_encrypt_key(key, 128, &ekey); 
	 AES_encrypt(msg, out, &ekey);
        if(!strcmp((char *)msg,"exit"))
        {
            //send to the client that server has closed the connection
            send(newSd, (char*)&out, strlen((char *)msg), 0);
            break;
        }
        //send the encrypted msg to client
        send(newSd, (char*)&out, strlen((char *)out), 0);
    }
    //we need to close the socket descriptors after we're all done
   
    close(newSd);
    close(serverSd);
   
    cout << "Connection closed..." << endl;
    return 0;
}


