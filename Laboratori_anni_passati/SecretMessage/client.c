#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MSG_LEN 50
#define MAX_HOST 5

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
    char msg[MSG_LEN] = "SECRET_MESSAGE1";

    if(argc != 4)   //  IP Port USR
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");
    
    memset(&addr, 0, len);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));

    bzero(sendline, BUFFER_SIZE);
    bzero(recvline, BUFFER_SIZE);

    if((connect(sockfd, (struct sockaddr*)&addr, len)) < 0)
        handle_error("Error connect\n");
    printf("[+]Connection established\n");

    //  C1 MSG 192.168.1.9-C1  .... 192.168.56.104-C2

    sprintf(sendline, "%s %s 192.168.1.9-C1 193.67.9.3-C4 192.168.56.104-C2", argv[3], msg);
    if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
        handle_error("Error send\n");
    printf("Secret message sent\n");
    
    for(;;)
    {
        // GET C1 C3 
        // CHANGE C1 NEW_MSG 192.168.1.9-C2  .... 192.168.56.104-C1
        printf("\nEnter a request:\n");
        fgets(sendline, BUFFER_SIZE, stdin);
        sendline[strcspn(sendline, "\n")] = 0;

        if(strcmp(sendline, "STOP") == 0)break;

        if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error send\n");

        if((n = recv(sockfd, recvline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error recv\n");
        recvline[n] = 0;
        printf("Reply: %s, IP:%s, Port:%d\n", recvline, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    printf("\nTransmission completed...\n");
    close(sockfd);

    return 0;
}