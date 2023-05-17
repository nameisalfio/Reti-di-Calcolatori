#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 2)
        handle_error("Error argc!\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    
    memset(&local_addr, 0, len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr *)&local_addr, len)) < 0)
        handle_error("Error bind\n");
    printf("Server listening...\n");

    for(;;)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&remote_addr, &len)) < 0)
            handle_error("Error recvfrom\n");
        buffer[n] = 0;

        printf("Message: %s IP: %s Port: %d\n", buffer, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
    }

    close(sockfd);
}