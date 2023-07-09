#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define USERNAME_SIZE 20
#define PASSWORD_SIZE 20
#define PORT 8000

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct 
{
    char request[BUFSIZ];
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char msg[BUFSIZ];
}Message;

void printMessage(Message* msg)
{
    printf("Message\n..........\n");
    printf("\nRequest : %s\n", msg->request);
    printf("Username : %s\n", msg->username);
    printf("Password : %s\n", msg->password);
    printf("Msg : %s\n\n", msg->msg);
}

int main(int argc, char* argv[])
{
    int sockfd, remote_sockfd, n;
    struct sockaddr_in6 server_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFSIZ];

    if(argc != 3)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    if((remote_sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, argv[1], &server_addr.sin6_addr);
    server_addr.sin6_port = htons(atoi(argv[2]));    

    memset(&remote_addr, 0, len);
    remote_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::", &remote_addr.sin6_addr);
    remote_addr.sin6_port = htons(PORT); 

    if(!fork())
    {
        Message msg;

        printf("Enter <START> : ");
        fgets(buffer, BUFSIZ, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if(strcasecmp(buffer, "START") == 0)
        {
            strcpy(msg.request, buffer);  //Request

            printf("\n-> Enter username: ");   //Username
            fgets(buffer, BUFSIZ, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(msg.username, buffer);   

            printf("-> Enter password: ");   //Password
            fgets(buffer, BUFSIZ, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(msg.password, buffer);   

            printf("-> Enter message: ");   //Message
            fgets(buffer, BUFSIZ, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(msg.msg, buffer);   

            if((sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*) &server_addr, len)) < 0)
                handle_error("Error sendto\n");
            
            if((n = recvfrom(sockfd, buffer, BUFSIZ, 0, (struct sockaddr*) &server_addr, &len)) < 0)
                handle_error("Error recvfrom\n");
            buffer[n] = 0;

            if(strcmp(buffer, "Registration failed") == 0) return 0;

            printf("\nToken assigned from server: %s\n", buffer);
            
            printf("\nDo you want to send you message? <YES> <NO> : ");
            fgets(buffer, BUFSIZ, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if(strcasecmp(buffer, "YES") == 0)
            {
                strcpy(msg.request, "SEND");
                if((sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*) &server_addr, len)) < 0)
                    handle_error("Error sendto\n");  

                printf("\n--------------------\n");      
            }

            close(sockfd);
        }
        else
        {
            printf("Invalid request\n");
            close(sockfd);
            return 0;
        }
    }
    else
    {
        if((bind(remote_sockfd, (struct sockaddr *)&remote_addr, len)) < 0)
            handle_error("Error bind\n");

        if((n = recvfrom(remote_sockfd, buffer, BUFSIZ, 0, (struct sockaddr*) &remote_addr, &len)) < 0)
            handle_error("Error recvfrom\n");
        buffer[n] = 0;
        printf("\n\n[+]Message received : %s\n", buffer);

        close(remote_sockfd);
    }


    return 0;
} 