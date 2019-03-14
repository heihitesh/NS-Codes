#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
// This header file defines some types of data structures which will be used for socket programming.
#include <sys/socket.h>
//This header contains the basic socket programming functions. Such as socket, bind, listen, accept, connect
#include <netinet/in.h>
//This header file defines the socket data structures such as sockaddr_in or sockaddr_in6
#include <arpa/inet.h>
//Some basic address conversion functions
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
//functions to obtain address info from a domain name.
#include <sys/uio.h>
//these are time utility headers.
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
//Client side

string encrypt(string data, int key)
{
  //This block of the code will be used for encryption and decryption of the code.
  string result;
  for (int i = 0; i < data.length();i++)
  {
    if(isalpha(data[i]) != 0)
    {
      if(isupper(data[i])) result += char(int(data[i]+key-65)%26 + 65);
      else result += char(int(data[i]+key -97)%26 + 97);
    }
    else result += data[i];

  }
  return result;
}
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
    //AF_INET is used for IPV4 doain. AF_INET6 will be used here if we are working with IPV6 obtain. But in that scenario the data structure will also change
    sendSockAddr.sin_addr = *(struct in_addr*)*host->h_addr_list;
    //host is a strucuture of hostnet type which stores the address relaeted info. This Information is obtained using above method and then stored in the adress into the socket data structure. 
    sendSockAddr.sin_port = htons(port);
    //It's necessary to convert the address from host byte order to network byte order(Big Endian). Hence conversion.
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //Here the socket is created and the rest of the process is just socket creation and listening.
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    // Here above described socket is used to listen to address descrived in the socket address. Client doesn't need to execute any other function.
    // Three variables used here are integer file descriptor, sockaddr type pointer to socket adddress and length of socketaddress in byte which is also an integer.
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; return 0;
    }
    cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    while(1)
    {
        cout << ">";
        string data;
        getline(cin, data);

        //Encrption goes here with key 3
        string result;
        result = encrypt(data,3);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, result.c_str());
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
        cout << "Awaiting server response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);

        //Decryption Goes here
        string recieved = msg;
        recieved = encrypt(recieved, 23);
        if(!strcmp(msg, "exit"))
        {
            cout << "Server has quit the session" << endl;
            break;
        }
        cout << "recieved: " << msg << endl;
        cout << "Server: " << recieved << endl;
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
