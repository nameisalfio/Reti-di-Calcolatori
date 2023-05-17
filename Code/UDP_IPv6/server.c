#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in6 client_addr, server_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];
    char ipv6_addr[INET6_ADDRSTRLEN];
    int ipv6_port;
    int n;

    if(argc != 2)
        handle_error("Error argc\n");
    
    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created succesful\n\n");

    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr *)&server_addr, len)) < 0)
        handle_error("[-]Error bind\n");
    printf("[+]Bind succesful\n");
    printf("\nServer listening...\n\n");

    for(;;)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len)) < 0)
            handle_error("[-]Error recvfrom\n");
        buffer[n] = 0;

        inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
        ntohs(client_addr.sin6_port);

        if(strncmp(buffer, "STOP", strlen("STOP")) == 0)
        {
            printf("Client %s disconnected from port %d\n", ipv6_addr, ipv6_port);
            continue;
        }

        printf("Message: %s\t IP: %s\t Port %d\n", buffer, ipv6_addr, ipv6_port);

        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, len)) < 0)
            handle_error("[-]Error sendto\n");

        printf("Reply : %s\n", buffer);
    }

    close(sockfd);
    
    return 0;
}