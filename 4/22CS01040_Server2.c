//
//  server.c
//  Client_Server
//

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

struct sfd{
    int sock_fd;
};

void *func(void* arg){
    int newsockfd = ((struct sfd*)arg)->sock_fd;
    ssize_t n;
    char buffer[256];

    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
}
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    ssize_t n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        
        pthread_t client_th;
        struct sfd* sock_sfd = (struct sfd*)(malloc (sizeof(struct sfd*)));
        sock_sfd->sock_fd = newsockfd;

        pthread_create(&client_th, NULL, func, (void*) sock_sfd);
     }
     
     return 0;
}
