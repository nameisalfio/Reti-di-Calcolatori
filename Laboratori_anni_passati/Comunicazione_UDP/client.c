#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8000

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, remote_sock, n;
    struct sockaddr_in server_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    memset(&remote_addr, 0, len);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &remote_addr.sin_addr);
    remote_addr.sin_port = htons(PORT);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <0)
        handle_error("Error socket\n");

    if((remote_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    if(!fork())
    {
        printf("Enter a request:\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer)-1] = 0;

        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &server_addr, len)) < 0)
            handle_error("Error sendto\n");
    }
    else
    {
        if((bind(remote_sock, (struct sockaddr*) &remote_addr, len)) < 0)
            handle_error("Error bind\n");

        if((n = recvfrom(remote_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &remote_addr, &len)) < 0)
            handle_error("Error recvfrom\n");
        buffer[n] = 0;

        printf("Message: %s\n\n", buffer);
        strcpy(buffer, "Message received succesful\n");

        if((sendto(remote_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &remote_addr, len)) < 0)
            handle_error("Error sendto\n");

        close(remote_sock);
    }

    close(sockfd);

    return 0;
} 
