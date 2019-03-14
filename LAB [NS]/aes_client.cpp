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
/* Following libray contains the methods for DES Encrption */
#include <openssl/aes.h>
using namespace std;
//Client side

#define BUFSIZE 4096

int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0);
    } //grab the IP address and port number
    char *serverIp = argv[1]; int port = atoi(argv[2]);
    //create a message buffer
    char msg[1500];
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr = *(struct in_addr*)*host->h_addr_list;
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; return 0;
    }
    cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);

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

        memset(&in, 0, sizeof(in));
        memset(&out, 0, sizeof(out));
        memset(&back, 0, sizeof(back));
        cout << ">";
        string data;
        getline(cin, data);
        strcpy(in, data.c_str());
        int len = strlen(in);
        memset(ivec, 0x00, AES_BLOCK_SIZE);
        AES_cbc_encrypt((unsigned char *)&in, (unsigned char *)&out, len,  &enc_key, ivec, AES_ENCRYPT);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, out);
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
        cout << "Awaiting server response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        memset(&out, 0, sizeof(out));
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);

        strcpy(out, msg);
        //Decryption Goes here
        len = strlen(out);

        memset(ivec, 0x00, AES_BLOCK_SIZE); // don't forget to set ivec vector again, else you can't decrypt data properly
        AES_cbc_encrypt((unsigned char *)&out, (unsigned char *)&back, len,  &dec_key, ivec, AES_DECRYPT);
        if(!strcmp(back, "exit"))
        {
            cout << "Server has quit the session" << endl;
            break;
        }
        cout << "recieved: " << msg << endl;
        cout << "Server: " << back << endl;
    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten <<
    " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec- start1.tv_sec)
      << " secs" << endl;
    cout << "Connection closed" << endl;
    return 0;
}
