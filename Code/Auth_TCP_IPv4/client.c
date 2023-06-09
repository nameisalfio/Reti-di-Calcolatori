#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define FIELD_SIZE 20

typedef struct 
{
    char op[FIELD_SIZE];
    char usr[FIELD_SIZE];
    char pass[FIELD_SIZE];
}Message;

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    int sockfd, n;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFSIZ];
    Message msg = {0};

    if(argc != 3)   handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  handle_error("Error socket\n");
    if((connect(sockfd, (struct sockaddr*) &server_addr, len)) < 0)  handle_error("Error connect\n");
    printf("Connection with server %s to port %d\n\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    
    while(true)
    {
        printf("Enter a request <REG> <LOG> <DEL> : ");
        fgets(buffer, BUFSIZ, stdin);
        buffer[strlen(buffer)-1] = 0;

        if(strcasecmp(buffer, "REG") == 0 || strcasecmp(buffer, "LOG") == 0 || strcasecmp(buffer, "DEL") == 0)
        {
            strcpy(msg.op, buffer);

            printf("Enter username : ");
            fgets(buffer, BUFSIZ, stdin);
            buffer[strlen(buffer)-1] = 0;
            strcpy(msg.usr, buffer);
            
            printf("Enter password : ");
            fgets(buffer, BUFSIZ, stdin);
            buffer[strlen(buffer)-1] = 0;
            strcpy(msg.pass, buffer);

            if((send(sockfd, &msg, sizeof(Message), 0)) < 0)    handle_error("Error send\n");
            if((n = recv(sockfd, buffer, BUFSIZ, 0)) < 0)  handle_error("Error recv\n");
            buffer[n] = 0;

            printf("Reply -> %s\n\n", buffer);
        }
        if(strcasecmp(buffer, "EXIT") == 0)
        {
            printf("Goodbye!\n\n");
            break;
        }
    }

    return 0;
}
