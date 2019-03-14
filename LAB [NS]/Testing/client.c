#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/des.h>
#include<openssl/rand.h>

#define port 4545
#define MAX 100

void fun(int sockfd){

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
  int i;
  for(;;){
    bzero(buff,sizeof(buff));
    bzero(out,sizeof(out));
    i = 0;
    while((buff[i++] = getchar()) != '\n');
    DES_ede3_cbc_encrypt((DES_cblock*)buff, (DES_cblock*)out,strlen(buff),&ksh1,&ksh2,&ksh3,&seed,DES_ENCRYPT);
    write(sockfd,out, sizeof(out));
    if(strncmp(buff,"exit",4) == 0){
      printf("EXIT");
      break;
    }

    bzero(buff,sizeof(buff));
    bzero(out,sizeof(out));

    read(sockfd,buff,sizeof(buff));
    DES_ede3_cbc_encrypt((DES_cblock*)buff, (DES_cblock*)out,strlen(buff),&ksh1,&ksh2,&ksh3,&seed,DES_DECRYPT);
    printf("\nS: %s",out);
  }
}

int main(){

  int sockfd;
  struct sockaddr_in servaddr;
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd != -1){
    printf("\nSocket Made success %d ", sockfd);
  }else
  printf("\nSocket Failed");

  bzero(&servaddr , sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(port);

  if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == 0){
    printf("\nConnected Successfully");
  }

  fun(sockfd);
  close(sockfd);

  return 0;
}
