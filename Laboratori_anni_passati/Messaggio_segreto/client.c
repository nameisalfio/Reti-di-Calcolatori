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
    char msg[MSG_LEN] = "Secret message";
    char usr[5];
    
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

    //  C1 MSG : 192.168.1.9-C1, .... , 192.168.56.104-C2

    sprintf(sendline, "%s %s : 192.168.1.9-C1, 192.168.56.104-C2", argv[3], msg);
    if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
        handle_error("Error send\n");
    printf("Secret message sent\n");

    if((n = recv(sockfd, recvline, BUFFER_SIZE, 0)) < 0)
        handle_error("Error recv\n");
    recvline[n] = 0;
    printf("Reply: %s, IP:%s, Port:%d\n", recvline, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    
    //  C1 GET_SECRET C4

    printf("\nWhose message do you want to know? (Enter ID) :\n");
    scanf("%s", usr);
    sprintf(sendline, "%s GET_SECRET %s", argv[3], usr);

    if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
        handle_error("Error send\n");

    if((n = recv(sockfd, recvline, BUFFER_SIZE, 0)) < 0)
        handle_error("Error recv\n");
    recvline[n] = 0;
    printf("Reply: %s, IP:%s, Port:%d\n", recvline, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    close(sockfd);
    return 0;
}