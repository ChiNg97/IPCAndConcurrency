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

// If typing character is !, it will end the s-talk
static int endOfMsg = 0;

pthread_t typingThrd, sendingThrd, receivingThrd, printingThrd;
pthread_mutex_t i_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t o_mutex = PTHREAD_MUTEX_INITIALIZER;
// buffAvail to signal the producer that buffer is available for adding more messages
// itemAvail to signal the consumer that item is availble for geting messages
pthread_cond_t i_buffAvail = PTHREAD_COND_INITIALIZER;
pthread_cond_t i_itemAvail = PTHREAD_COND_INITIALIZER;
pthread_cond_t o_buffAvail = PTHREAD_COND_INITIALIZER;
pthread_cond_t o_itemAvail = PTHREAD_COND_INITIALIZER;

void freeItem(void* item)
{
    free(item);
}

void endThrd () {
    shutdown(sockfd,2);
    pthread_exit(NULL);
}

void* typing(LIST* i_msgList) {
    while (!endOfMsg) {
        char typeMsg[BUFF_MAX_SIZE];

        // Read from keyboard
        int numBytes = read(STDIN_FILENO, &typeMsg, BUFF_MAX_SIZE);
        typeMsg[numBytes-1] = '\0';

        // Critical section
        pthread_mutex_lock(&i_mutex);
        if (ListCount(i_msgList) == LIST_MAX_SIZE)
            pthread_cond_wait(&i_buffAvail,&i_mutex);
        ListPrepend(i_msgList, (void*)typeMsg);
        pthread_cond_signal (&i_itemAvail);
        pthread_mutex_unlock(&i_mutex);

        if (strcmp(typeMsg,"!") == 0) {
            sleep(0.8);
            endOfMsg = 1;
            pthread_exit(NULL);
        }  
    }

    pthread_exit(NULL);
    return NULL;
}

void* sending(LIST* i_msgList) {
    while (!endOfMsg) {
        char* sendMsg;
        
        if(strcmp(sendMsg,"!") == 0) {
            endOfMsg = 1;
            endThrd();
        }

        // Critical section
        pthread_mutex_lock(&i_mutex);
        if (ListCount(i_msgList) == 0) 
            pthread_cond_wait(&i_itemAvail,&i_mutex);
        sendMsg = (char*) ListTrim(i_msgList);
        pthread_cond_signal(&i_buffAvail);
        pthread_mutex_unlock(&i_mutex);

        // Sending
        int sendReturn = sendto(sockfd, sendMsg, strlen(sendMsg)+1, 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
        if (sendReturn == -1) {
            printf("Can not send\n");
            printf("Sending error: %s\n", strerror(errno));
            endOfMsg=1;
            endThrd();
        }
    }

    endThrd();
    return NULL;
}

void* receiving(LIST* o_msgList) {
    while (!endOfMsg) {
        char receiveMsg[BUFF_MAX_SIZE];

        // Receiving
        socklen_t sendSize = sizeof(server);
        int recvReturn = recvfrom(sockfd,&receiveMsg,BUFF_MAX_SIZE,0,(struct sockaddr*)&server,&sendSize);
        if (recvReturn == -1) {
            printf("Can not receive\n");
            printf("Receiving error: %s\n", strerror(errno));
            endThrd();
        }

        receiveMsg[strlen(receiveMsg)] = '\0';

        // Critical section
        pthread_mutex_lock(&o_mutex);
        if (ListCount(o_msgList) == LIST_MAX_SIZE)
            pthread_cond_wait(&o_buffAvail,&o_mutex);
        ListAppend(o_msgList, (void*)receiveMsg);
        pthread_cond_signal (&o_itemAvail);
        pthread_mutex_unlock(&o_mutex);

        
        if(strcmp(receiveMsg,"!") == 0) {
            endOfMsg = 1;
            endThrd();
        }
    }

    endThrd();
    return NULL;
}

void* printing(LIST* o_msgList) {
    while (!endOfMsg) {
        char* printMsg;

        // Critical section
        pthread_mutex_lock(&o_mutex);
        if (ListCount(o_msgList) == 0) 
            pthread_cond_wait(&o_itemAvail,&o_mutex);
        printMsg = (char*)ListTrim(o_msgList);
        pthread_cond_signal(&o_buffAvail);
        pthread_mutex_unlock(&o_mutex);

        // Printing
        printf("Message: %s\n", printMsg);

        if (strcmp(printMsg,"!") == 0) {
            endOfMsg = 1;
            pthread_exit(NULL);
        }
    }
        
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {

    // Set up ports
    int port_1 = (int)(long)(argv[1]);
    printf("PORT: %i\n", port_1);
    //memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_1);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int port_2 = (int)(long)(argv[3]);
    char* clientName = argv[2];
    struct hostent* clientHost;
    clientHost = gethostbyname(clientName);
    //memset(&client, 0, sizeof(struct sockaddr_in));
    client.sin_family = AF_INET;
    client.sin_port = htons(port_2);
    client.sin_addr.s_addr = **(clientHost->h_addr_list);
    
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (sockfd == -1) {
        printf("Can not create socket\n");
        printf("Socket error: %s\n", strerror(errno));
    }

    int bindReturn = bind(sockfd,(struct sockaddr*)&server, sizeof(struct sockaddr_storage));
    if (bindReturn == -1) {
        printf("Can not bind\n");
        printf("Binding error: %s\n", strerror(errno));
    }

    // Exchanging messages
    LIST* i_msgList = ListCreate();
    LIST* o_msgList = ListCreate();

    pthread_create(&typingThrd,NULL,(void*)&typing,(void*)i_msgList);
    pthread_create(&sendingThrd,NULL,(void*)&sending,(void*)i_msgList);
    pthread_create(&receivingThrd,NULL,(void*)&receiving,(void*)o_msgList);
    pthread_create(&printingThrd,NULL,(void*)&printing,(void*)o_msgList);

    pthread_join(typingThrd, NULL);
    pthread_join(sendingThrd, NULL);
    pthread_join(receivingThrd, NULL);
    pthread_join(printingThrd, NULL);

    pthread_mutex_destroy(&i_mutex);
    pthread_mutex_destroy(&o_mutex);

    ListFree(i_msgList, freeItem);
    ListFree(o_msgList, freeItem);
  
    return 0;
}