#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define ID_SIZE 10
#define PORT 8000

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, remote_sock, n;
    struct sockaddr_in server_addr, client_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    char IPv4[INET_ADDRSTRLEN];
    char ID[ID_SIZE];

    if(argc != 2)
        handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    memset(&remote_addr, 0, len);

    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[1]));

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    if((remote_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0)
        handle_error("Error bind\n");

    FILE* fp;
    if(!(fp = fopen("Set.txt", "r")))
        handle_error("Error fopen\n");

    printf("[+]Server listening ...\n\n");
    while(true)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &client_addr, &len)) < 0)
            handle_error("Error recvfrom\n");
        buffer[n] = 0;
        printf("Client : %s\n", buffer);

        while(fscanf(fp, "%s %s", IPv4, ID) == 2)
        {
            inet_pton(AF_INET, IPv4, &remote_addr.sin_addr);

            if((sendto(remote_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &remote_addr, len)) < 0)
                handle_error("Error sendto\n");

            if((n = recvfrom(remote_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &remote_addr, &len)) < 0)
                handle_error("Error recvfrom\n");

            buffer[n] = 0;
            printf("Reply from %s --> %s\n", ID, buffer);
        }
    }

    fclose(fp);
    close(sockfd);
    close(remote_sockfd);

    return 0;
} 
