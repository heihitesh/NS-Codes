#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define portClient 4545
#define portServer 4546
#define MAX 100


void fun(int sockfd_c, int sockfd_s){
  char buff[MAX];
  int i;
  for(;;){
    //read data from client
    bzero(buff,sizeof(buff));
    read(sockfd_s,buff,sizeof(buff));
    printf("MSG from Client: %s",buff);
    write(sockfd_c,buff,sizeof(buff));
    bzero(buff,sizeof(buff));

    read(sockfd_c,buff,sizeof(buff));
    printf("Msg from Server: %s",buff);
    write(sockfd_s,buff,sizeof(buff));
  }
}

int main(){
  //working as a Client
  int sockfd;
  struct sockaddr_in servaddr;
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd != -1){
    printf("Connected Success %d",sockfd);
  }
  bzero(&servaddr,sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(portServer);

  if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == 0){
    printf("Connected to Server");
  }

  //working as a Server
  int sockfd_c;
  struct sockaddr_in servaddr1,cli;
  sockfd_c = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd_c != -1){
    printf("Connected Success %d",sockfd_c);
  }
  bzero(&servaddr1,sizeof(servaddr1));

  servaddr1.sin_family = AF_INET;
  servaddr1.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr1.sin_port = htons(portClient);

  if(bind(sockfd_c,(struct sockaddr*)&servaddr1,sizeof(servaddr1)) == 0){
    printf("Binded to Server");
  }
  listen(sockfd_c,5);

  int len = sizeof(cli);
  int clientID = accept(sockfd_c,(struct sockaddr*)&cli,&len);

  fun(sockfd,clientID);

  return 0;
}
