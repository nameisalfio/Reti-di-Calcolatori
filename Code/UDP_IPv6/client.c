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
    struct sockaddr_in6 client_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];
    char ipv6_addr[INET6_ADDRSTRLEN];
    int ipv6_port;
    int n;

    if(argc != 3)
        handle_error("Error argc\n");
    
    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created succesful\n\n");

    memset(&client_addr, 0, len);
    client_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, argv[1], &(client_addr.sin6_addr));
    client_addr.sin6_port = htons(atoi(argv[2]));

    for(;;)
    {
        printf("Enter a request:\n");
        fgets(buffer, BUFFER_SIZE, stdin);

        if(strncmp(buffer, "STOP", strlen("STOP")) == 0) break;

        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, len)) < 0)
            handle_error("[-]Error sendto\n");

        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len)) < 0)
            handle_error("[-]Error recvfrom\n");
        buffer[n] = 0;

        inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
        ipv6_port = htons(client_addr.sin6_port);

        printf("Reply: %s\t IP: %s\t Port %d\n", buffer, ipv6_addr, ipv6_port);
    }

    close(sockfd);
    
    return 0;
}