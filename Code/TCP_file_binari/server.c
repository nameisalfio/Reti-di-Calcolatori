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
    exit(1);
}

int main(int argc, char* argv[])
{
    int sockfd, sockfd_client;
    struct sockaddr_in client_addr, server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    int n;

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created successful\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*)&server_addr, len)) < 0)
        handle_error("Error bind\n");

    printf("Server listening...\n");
    listen(sockfd, 5);

    for(;;) 
    {
        if((sockfd_client = accept(sockfd, (struct sockaddr*)&client_addr, &len)) < 0)
            handle_error("[-]Error accept\n");
        printf("[+]Client %s connected to port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        do
        {
            if((n = recv(sockfd_client, buffer, sizeof(buffer), 0)) < 0)
                handle_error("[-]Error recv\n");
            if(n <= 0)break;
            buffer[n] = 0;
            printf("\nMessage received:\n%s  IP: %s  Port %d\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            if((send(sockfd_client, "ACK", strlen("ACK"), 0)) < 0)
                handle_error("[-]Error send\n");

        }while(n > 0);

        close(sockfd_client);
    }
    close(sockfd);

    return 0;
}