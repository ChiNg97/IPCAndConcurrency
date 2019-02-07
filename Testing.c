#include "list.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFF_MAX_SIZE 100
#define LIST_MAX_SIZE 100

struct sockaddr_in server;
struct sockaddr_in client;
int sockfd;


int main() {
    char *sendMsg = "hello";
    // Set up ports
    int port_1 = 1000;
    printf("PORT1: %i\n", port_1);
    //memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_1);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int port_2 = 1008;
    printf("PORT2: %i\n", port_2);

    char* clientName = "127.0.0.1";
    struct hostent* clientHost;
    clientHost = gethostbyname(clientName);
    client.sin_family = AF_INET;
    client.sin_port = htons(port_2);
    client.sin_addr.s_addr = *(clientHost->h_addr);


    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    bind(sockfd,(struct sockaddr*)&server, sizeof(struct sockaddr_in));
    sendto(sockfd, sendMsg, strlen(sendMsg)+1, 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
    printf("--5\n");

    unsigned int sendSize = sizeof(struct sockaddr_in);
    printf("--6\n");

    char buffer[1000];
   int i =  recvfrom(sockfd,&buffer,BUFF_MAX_SIZE,0,(struct sockaddr*)&server,&sendSize);
   if (i == -1) {
    printf("Error: %s\n", strerror(errno) );
   }
    printf("--7\n");

    printf("%s\n", buffer);



  
    return 0;
}