//SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    strcat(msg, "\n");
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char*argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket");
    printf("[+]Socket created successfully\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, &(addr.sin_addr));
    addr.sin_port = htons(PORT);

    if(bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        handle_error("[-Error bind]");
    printf("[+]Binding socket\n\n");

    printf("Server listening...\n");

    socklen_t size = sizeof(addr);
    ssize_t rf;
    if((rf = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &addr, &size))< 0)
        handle_error("[-]Error sendto");

    buffer[rf] = '\0';
    printf("Data received: %s\n", buffer);

    close(sockfd);

    return 0;
}