#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char ipv6_addr[INET6_ADDRSTRLEN];
    ssize_t n;

    if(argc < 2)
        handle_error("Too few argument to main function\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM | SOCK_NONBLOCK, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(atoi(argv[1]));

    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        handle_error("[-]Error bind\n");
    printf("[+]Server listening on port %s...\n", argv[1]);
/*
    for(;;)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len)) < 0)
            handle_error("[-]Error recvfrom\n");

        buffer[n] = 0;
        inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
        if(strncmp("STOP", buffer, strlen("STOP")) == 0)
        {
            sendto(sockfd, "STOP", strlen("STOP"), 0, (struct sockaddr *)&client_addr, len);
            printf("\nTerminating connection with client %s ...\n", ipv6_addr);
            continue;
        }

        printf("\nMessage: %s\t IP: %s\t Port %d\n", buffer, ipv6_addr, ntohs(client_addr.sin6_port));

        if((sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, len)) < 0)
            handle_error("[-]Error sendto\n");
        printf("[+]Reply sent to client %s\n", ipv6_addr);
    }
*/
    for(;;)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len)) < 0)
        {
            if(errno == EWOULDBLOCK)  // nessun dato disponibile
            {
                printf("\nWaiting for incoming connections...\n");
                printf("\n");
                sleep(5); // pausa di 5 secondi
                continue;
            }
            else
                handle_error("[-]Error recvfrom\n");
        }

        buffer[n] = 0;
        inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
        if(strncmp("STOP", buffer, strlen("STOP")) == 0)
        {
            sendto(sockfd, "STOP", strlen("STOP"), 0, (struct sockaddr *)&client_addr, len);
            printf("\nTerminating connection with client %s ...\n", ipv6_addr);
            continue;
        }

        printf("Message: %s\t IP: %s\t Port %d\n", buffer, ipv6_addr, ntohs(client_addr.sin6_port));

        if((sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, len)) < 0)
            handle_error("[-]Error sendto\n");
        printf("[+]Reply sent to client %s\n", ipv6_addr);
    }

    close(sockfd);

    return 0;
}
