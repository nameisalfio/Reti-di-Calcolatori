#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

#define handle(msg)                             \
do{                                             \
    fprintf(stderr, "[%d] error ", __LINE__);   \
    perror(msg);                                \
    exit(EXIT_FAILURE);                         \
}while (false);

#define USR_SIZE 20
#define PORT 8000

typedef struct 
{
    char usr[USR_SIZE];
    char pass[INET6_ADDRSTRLEN];
    char msg[BUFSIZ];
    int port;
}Message;

int main(int argc, char* argv[])
{
    int sock, n, token;
    struct sockaddr_in6 server_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFSIZ];
    Message msg;

    if(argc != 3) handle("argc");

    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, argv[1], &server_addr.sin6_addr);
    server_addr.sin6_port = htons(atoi(argv[2]));

    if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) handle("socket");

    if(fork())
    {
        printf("Enter username: "); 
        fgets(msg.usr, USR_SIZE, stdin);
        msg.usr[strcspn(msg.usr, "\n")] = 0;
        msg.port = PORT;
        strcpy(msg.msg, "");

        if((sendto(sock, &msg, sizeof(Message), 0, (struct sockaddr*)&server_addr, len)) < 0) handle("sendto");
        if((n = recvfrom(sock, buffer, BUFSIZ, 0, (struct sockaddr*)&server_addr, &len)) < 0) handle("recvfrom");
        buffer[n] = 0;

        if(!strcmp(buffer, "Register failed")) handle(buffer);
        token = atoi(buffer);
        printf("Token: %d\n", token);

        printf("Do you want send a message? <y> <n> : ");
        char choise = getchar();
        while(getchar() != '\n'){}
        if(choise == 'n') wait(NULL); // Remains listening

        while(true)
        {
            printf("\nEnter message: "); 
            fgets(buffer, BUFSIZ, stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            sprintf(msg.msg, "%d ", token); // Token Message
            strcat(msg.msg, buffer);

            if(!strcasecmp(buffer, "EXIT")) break;
            if((sendto(sock, &msg, sizeof(Message), 0, (struct sockaddr*)&server_addr, len)) < 0) handle("sendto");
        }
        close(sock);
        exit(EXIT_SUCCESS);
    }   
    else
    {
        int remote_sock;
        struct sockaddr_in6 remote_addr;

        memset(&remote_addr, 0, len);
        remote_addr.sin6_family = AF_INET6;
        inet_pton(AF_INET6, "::", &remote_addr.sin6_addr);
        remote_addr.sin6_port = htons(PORT);
        if((remote_sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) handle("socket");
        if((bind(remote_sock, (struct sockaddr*)&remote_addr, len)) < 0) handle("");

        while(true)
        {
            if((n = recvfrom(remote_sock, buffer, BUFSIZ, 0, (struct sockaddr*)&remote_addr, &len)) < 0) handle("recvfrom");
            buffer[n] = 0;
            printf("\nMessage: %s\n", buffer);
        }
        close(remote_sock);
        exit(EXIT_SUCCESS);
    }
}