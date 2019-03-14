#include <iostream>
#include <string>
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
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
//Following libray contains the methods for DES Encrption
#include <openssl/aes.h>


using namespace std;
//Server side
#define BUFSIZE 4096

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
    char msg[1500];

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
    //lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    //also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;

    char in[BUFSIZE], out[BUFSIZE], back[BUFSIZE];

    /* AES key for Encryption and Decryption */
    const static unsigned char aes_key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

    /* AES-128 bit CBC Encryption */
	   AES_KEY enc_key, dec_key;
	   AES_set_encrypt_key(aes_key, sizeof(aes_key)*8, &enc_key);
     unsigned char ivec[AES_BLOCK_SIZE];
     AES_set_decrypt_key(aes_key, sizeof(aes_key)*8, &dec_key); // Size of key is in bits

    while(1)
    {
        memset(in, 0, sizeof(in));
        memset(out, 0, sizeof(out));
        memset(back, 0, sizeof(back));

        //receive a message from the client (listen)
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        strcpy(out, msg);
        int len = strlen(out);
        //DES_ecb_encrypt((C_Block *)out,(C_Block *)back, &keysched, DES_DECRYPT);
        memset(ivec, 0x00, AES_BLOCK_SIZE);
        AES_cbc_encrypt((unsigned char *)&out, (unsigned char *)&back, len,  &dec_key, ivec, AES_DECRYPT);
        if(!strcmp(back, "exit"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        cout << "recieved: " << msg << endl;
        cout << "Client: " << back << endl;
        cout << ">";
        string data;
        getline(cin, data);
        strcpy(in, data.c_str());
        memset(out, 0, sizeof(out));
        len = strlen(in);

        memset(ivec, 0x00, AES_BLOCK_SIZE);
        AES_cbc_encrypt((unsigned char *)&in, (unsigned char *)&out, len,  &enc_key, ivec, AES_ENCRYPT);
        memset(&msg, 0, sizeof(msg)); //clear the buffer
        strcpy(msg, out);
        if(data == "exit")
        {
            //send to the client that server has closed the connection
            send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        //send the message to client
        bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    }
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
        << " secs" << endl;
    cout << "Connection closed..." << endl;
    return 0;
}
