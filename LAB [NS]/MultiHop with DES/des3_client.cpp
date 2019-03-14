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
#include <openssl/des.h>
#include <openssl/rand.h>
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
    char *e = out;

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

        memset(&in, 0, sizeof(in));
        memset(&out, 0, sizeof(out));
        memset(&back, 0, sizeof(back));
        cout << ">";
        string data;
        getline(cin, data);

        //Encrption goes here with key 3
        strcpy(in, data.c_str());
        int len = strlen(in);
        DES_ede3_cbc_encrypt((unsigned char *)&in, (unsigned char *)&out, len, &keysched1,&keysched2, &keysched3, &ivec, DES_ENCRYPT);
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
        DES_ede3_cbc_encrypt((unsigned char *)&out, (unsigned char *)&back, len,&keysched1,&keysched2, &keysched3, &ivec, DES_DECRYPT);
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
