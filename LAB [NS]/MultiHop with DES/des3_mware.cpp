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
#include <openssl/des.h>

#define BUFSIZE 4096

using namespace std;
//Server side

int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 3)
    {
        cerr << "Usage: 2 ports" << endl;
        exit(0);
    }
    //grab the port number
    int serverPort = atoi(argv[1]);
    int clientPort = atoi(argv[2]);

    if(clientPort == serverPort)
    {
      cout << "Different port numbers are required for multi hop connection";
      return 0;
    }

    //buffer to send and receive messages with
    char msg[1500];

    //setup a socket and connection tools
    sockaddr_in servAddr;
    sockaddr_in clientAddr;
    bzero((char *)&clientAddr, sizeof(clientAddr));
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(serverPort);

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(clientPort);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0 || clientSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    int status = connect(serverSd, (sockaddr *)&servAddr, sizeof(servAddr));
    if (status < 0)
    {
      cout <<"Error connecting to the main server\n";
      exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(clientSd, (struct sockaddr*) &clientAddr,
        sizeof(clientAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(clientSd, 2);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client
    int newSd = accept(clientSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
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

    /* Triple DES - 3 key CBC Encryption */
    DES_cblock key1;
    DES_cblock key2 = {0x11, 0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    DES_cblock key3 = {0x00, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    DES_cblock seed = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    DES_key_schedule keysched1, keysched2, keysched3;
    DES_cblock ivec = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    DES_string_to_key("Sample Key",&key1);

    DES_set_key((C_Block *)key1, &keysched1);
    DES_set_key((C_Block *)key2, &keysched2);
    DES_set_key((C_Block *)key2, &keysched2);

    while(1)
    {
        //receive a message from the client (listen)
        cout << "Awaiting client response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        memset(&in, 0, sizeof(in));
        memset(&out, 0, sizeof(out));

        //Decryption Goes here
        bytesRead += recv(newSd, (char*)&msg, sizeof(msg), 0);
        strcpy(in, msg);
        memset(ivec, 0x00, sizeof(ivec));
        DES_ede3_cbc_encrypt((unsigned char *)&out, (unsigned char *)&in, strlen(msg),&keysched1,&keysched2, &keysched3, &ivec, DES_DECRYPT);
        // recieved = encrypt(recieved, 23);
        string recieved = out;
        if(recieved ==  "exit")
        {
            cout << "Client has quit the session" << endl;
            bytesWritten += send(serverSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        cout << "Client: " << out << endl;
        cout << ">";
        bytesWritten += send(serverSd, (char*)&msg, strlen(msg), 0);

        memset(&msg, 0, sizeof(msg)); //clear the buffer
        memset(&in, 0, sizeof(in));
        memset(&out, 0, sizeof(out));

        bytesRead += recv(serverSd, (char*)&msg, sizeof(msg),0);
        strcpy(in, msg);
        memset(ivec, 0x00, sizeof(ivec));
        DES_ede3_cbc_encrypt((unsigned char *)&out, (unsigned char *)&in, strlen(msg),&keysched1,&keysched2, &keysched3, &ivec, DES_DECRYPT);
        recieved = out;
        // recieved = encrypt(recieved, 23);
        if(recieved == "exit")
        {
            //send to the client that server has closed the connection
            bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        //send the message to client
        cout << "Server: " << out  <<endl;
        bytesWritten += send(newSd, (char*)&msg, strlen(msg), 0);
    }
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);
    close(clientSd);
    cout << "********Session********" << endl;
    cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << endl;
    cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec)
        << " secs" << endl;
    cout << "Connection closed..." << endl;
    return 0;
}
