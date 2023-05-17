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
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    int n = 0;

    if(argc != 3)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created successfully\n");

    memset(&client_addr, 0, len);
    client_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &client_addr.sin_addr);
    client_addr.sin_port = htons(atoi(argv[2]));

    if((connect(sockfd, (struct sockaddr*)&client_addr, len)) < 0)
        handle_error("[-]Error connect\n");
    printf("[+]Connected to the server\n\n");

    for(;;)
    {
        printf("Enter a request in the following format . <REQ> <Username> <Password>\n");
        fgets(buffer, BUFFER_SIZE, stdin);

        if((send(sockfd, buffer, strlen(buffer), 0)) < 0)
                handle_error("[-]Error send\n");
        if(strncmp(buffer, "STOP\n", strlen("STOP\n")) == 0) break;

        if((n = recv(sockfd, buffer, sizeof(buffer), 0)) < 0)
            handle_error("[-]Error recv\n");
        buffer[n] = 0;

       printf("Reply: %s\t IP: %s\t Port %d\n\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    printf("\nTransmission completed...\n");
    close(sockfd);

    puts("File:");
    system("cat users.txt");

    return 0;
}