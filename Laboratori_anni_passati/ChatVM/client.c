#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
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
    int sockfd, n;
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&addr, 0, len);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));

    bzero(sendline, BUFFER_SIZE);
    bzero(recvline, BUFFER_SIZE);

    if((connect(sockfd, (struct sockaddr *)&addr, len)))
        handle_error("Error connect\n");

    for(;;)
    {
        printf("Enter a request:\n");
        fgets(sendline, BUFFER_SIZE, stdin);
        sendline[strcspn(sendline, "\n")] = 0;

        if(strncmp(sendline, "STOP", strlen("STOP")) == 0)break;

        if(send(sockfd, sendline, BUFFER_SIZE, 0) < 0)
            handle_error("Error send\n");

        if(n = recv(sockfd, recvline, BUFFER_SIZE, 0) < 0)
            handle_error("Error recv\n");
        recvline[n] = 0;
        printf("Reply: %s, IP: %s, Port: %d\n\n", recvline, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    close(sockfd);

    return 0;
}