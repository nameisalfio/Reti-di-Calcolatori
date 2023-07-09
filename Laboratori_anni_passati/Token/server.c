#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define USERNAME_SIZE 20
#define PASSWORD_SIZE 20
#define PORT 8000

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

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool LOG(Message* msg, FILE* fp)
{
    rewind(fp);
    char line[BUFSIZ];
    while(fgets(line, BUFSIZ, fp))
    {
        char* l_username = strtok(line, " ");
        if (strcmp(l_username, msg->username) == 0)
            return true;
    }
    return false;
}

bool REG(Message* msg, FILE* fp, int* token, char* ipv6)
{
    if (LOG(msg, fp))
        return false;

    fseek(fp, 0, SEEK_END);
    srand(time(NULL));  //seed
    *token = rand();
    fprintf(fp, "%s %s %d %d %s\n", msg->username, msg->password, PORT, *token, ipv6);
    fflush(fp);
    return token;
}

void SEND(Message* msg, FILE* fp)
{
    rewind(fp);

    int remote_sockfd;
    struct sockaddr_in6 remote_addr;
    socklen_t len = sizeof(struct sockaddr_in6);

    if((remote_sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&remote_addr, 0, len);
    remote_addr.sin6_family = AF_INET6;
    remote_addr.sin6_port = htons(PORT);

    char usr[USERNAME_SIZE];
    char pass[PASSWORD_SIZE];
    int port_number;
    int token;
    char ipv6[INET6_ADDRSTRLEN];

    while (fscanf(fp, "%s %s %d %d %s", usr, pass, &port_number, &token, ipv6) == 5)
    {
        if (strcmp(msg->username, usr) != 0)
        {
            inet_pton(AF_INET6, ipv6, &remote_addr.sin6_addr);
            if (sendto(remote_sockfd, msg->msg, BUFSIZ, 0, (struct sockaddr*)&remote_addr, len) < 0)
                handle_error("Error sendto\n");
        }
    }
}

char* handle_request(Message* msg, FILE* fp, char* ipv6)
{
    if(strcasecmp(msg->request, "START") == 0)
    {
        int token=0;
        if(REG(msg, fp, &token, ipv6))    //prima fase di registrazione
        {
            char* str = malloc(BUFSIZ);
            sprintf(str, "%d", token);
            return str;
        }
        return "Registration failed";
    }
    if(strcasecmp(msg->request, "SEND") == 0)   //gestione dell'invio ai client
    {
        SEND(msg, fp);
        return "Message sent";
    }
}

int main(int argc, char* argv[])
{
    int sockfd, n, port_client;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFSIZ];

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&client_addr, 0, len);
    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(atoi(argv[1]));  

    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0)
        handle_error("Error bind\n");  
    printf("[+]Server listening...\n\n");

    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    while(true)
    {
        Message msg;
        if((n = recvfrom(sockfd, &msg, sizeof(Message), 0, (struct sockaddr*) &client_addr, &len)) < 0)
            handle_error("Error recvfrom\n"); 

        char ipv6[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &client_addr.sin6_addr, ipv6, INET6_ADDRSTRLEN);
        
        strcpy(buffer, handle_request(&msg, fp, ipv6));   //Elaborazione
        printf("Outcome operation: %s\n\n", buffer);

        if((sendto(sockfd, buffer, BUFSIZ, 0, (struct sockaddr* )&client_addr, len)) < 0)
            handle_error("Error sendto\n");
    }
    fclose(fp);
    close(sockfd);

    return 0;
} 