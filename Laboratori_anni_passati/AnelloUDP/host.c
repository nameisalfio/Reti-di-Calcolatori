#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define START_POINT 'A'

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFSIZ];
    char recvline[BUFSIZ];
    char IPv4[INET_ADDRSTRLEN];

    if(argc != 2)   handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)   handle_error("Error socket\n");
    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0)  handle_error("Error bind\n");

    printf("Waiting for message from host %c...\n\n", START_POINT);
    while(true)
    {
        if((n = recvfrom(sockfd, recvline, BUFSIZ, 0, (struct sockaddr*) &client_addr, &len)) < 0)  handle_error("Error recvfrom\n");
        recvline[n] = 0;
        if((sendto(sockfd, "ACK", BUFSIZ, 0, (struct sockaddr*) &client_addr, len)) < 0)   handle_error("Error sendto\n");
        printf("ACK sent to host %c\n", START_POINT);
    }
    close(sockfd);

    return 0;
}