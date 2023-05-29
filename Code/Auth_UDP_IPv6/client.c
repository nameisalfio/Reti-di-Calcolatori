#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define USERNAME_SIZE 20
#define PASSWORD_SIZE 20

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct 
{
    char operation[BUFFER_SIZE];
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
}Message;

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in6 server_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, argv[1], &server_addr.sin6_addr);
    server_addr.sin6_port = htons(atoi(argv[2]));    

    while(true)
    {   
        Message msg;

        printf("Enter a request (REG, LOG, DEL, EXIT):\n");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if(strcasecmp(buffer, "REG") == 0 || strcasecmp(buffer, "LOG") == 0 || strcasecmp(buffer, "DEL") == 0)
        {
            strcpy(msg.operation, buffer);  //Operation

            printf("\n-> Enter username:\n");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(msg.username, buffer);   //Username

            printf("-> Enter password:\n");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(msg.password, buffer);   //Password

            if((sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*) &server_addr, len)) < 0)
                handle_error("Error sendto\n");
            
            if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &server_addr, &len)) < 0)
                handle_error("Error recvfrom\n");
            printf("\nReply from server: %s\n\n", buffer);
        }
        else if(strcasecmp(buffer, "EXIT") == 0)
        {
            printf("Goodbye!\n");
            close(sockfd);
            break;
        }
        else
            printf("Invalid request\n");
    }

    return 0;
} 
