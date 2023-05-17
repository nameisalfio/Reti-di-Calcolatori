#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(1);
}

bool register_user(char* username, char* password)
{
    FILE* fp; 
    if(!(fp = fopen("users.txt", "a+")))
        handle_error("Error fopen\n");

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, fp))
    {
        char* usr = strtok(line, " ");
        char* pass = strtok(NULL, "\n");
        if (strcmp(usr, username) == 0 && strcmp(pass, password) == 0) 
        {
            fclose(fp);
            return false;
        }
    }
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
    return true;
}

bool remove_user(char* username, char* password)
{
    FILE* fp;
    if (!(fp = fopen("users.txt", "r")))
        handle_error("Error fopen\n");

    FILE* fp_temp;
    if (!(fp_temp = fopen("temp_file", "w")))
        handle_error("Error fopen\n");

    char line[BUFFER_SIZE];
    bool removed = false;
    while (fgets(line, BUFFER_SIZE, fp))
    {
        char* usr = strtok(line, " ");
        char* pass = strtok(NULL, "\n");
        if (strcmp(usr, username) == 0 && strcmp(pass, password) == 0) 
            removed = true;

        else 
            fprintf(fp_temp, "%s %s\n", usr, pass);
    }
    fclose(fp);
    fclose(fp_temp);

    if (removed) 
    {
        if (remove("users.txt") != 0)    //rimuovo il vecchio file
            handle_error("Error remove\n");

        if (rename("temp_file", "users.txt") != 0)   //rinomino il nuovo file 
            handle_error("Error rename\n");
        return true;
    } 
    else 
    {
        remove("temp_file");
        return false;
    }
}

bool login_user(char* username, char* password)
{
    FILE* fp; 
    if(!(fp = fopen("users.txt", "r")))
        handle_error("Error fopen\n");
    
    char line[BUFFER_SIZE];
    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* usr = strtok(line, " ");
        char* pass = strtok(NULL, "\n");
        if (strcmp(usr, username) == 0 && strcmp(pass, password) == 0) 
        {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t len = sizeof(client_addr);
    ssize_t n;
    char ipv6_addr[INET6_ADDRSTRLEN];   //46
    char buffer[BUFFER_SIZE];
    char str[BUFFER_SIZE];
    char* request;
    char* reply;
    char* username;
    char* password;
    int numeroFail = 0;

    if(argc < 2)
        handle_error("Too few argument to main function\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(atoi(argv[1]));

    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        handle_error("[-]Error bind\n");
    printf("[+]Server listening on port %s...\n", argv[1]);

    for(;;)
    {
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len)) < 0)
            handle_error("[-]Error recvfrom\n");

        buffer[n] = 0;
        inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipv6_addr, INET6_ADDRSTRLEN);
        printf("\nMessage: %s  IP: %s  Port %d\n", buffer, ipv6_addr, ntohs(client_addr.sin6_port));

        strncpy(str, buffer, strlen(buffer));
        request = strtok(str, "_");
        username = strtok(NULL, "_");
        password = strtok(NULL, "\n");
        reply = malloc(BUFFER_SIZE);

        if (strcmp(request, "REG") == 0)
        {
            if (register_user(username, password)) 
            {
                printf("Registration of user %s successful\n", username);
                sprintf(reply, "Registration of user %s successful", username);
            }
            else 
            {
                printf("User %s already registered\n", username);
                sprintf(reply, "User %s already registered\n", username);
            }
        } 
        
        else if (strcmp(request, "RMV") == 0) 
        {
            if (remove_user(username, password))
            {
                printf("Removal of user %s successful\n", username);
                sprintf(reply, "Removal of user %s successful", username);
            }
            else
            {
                printf("Removal of user %s failed\n", username);
                sprintf(reply, "Removal of user %s failed", username);
            }
        }
        
        else if (strcmp(request, "LOG") == 0) 
        {
            if (login_user(username, password)) 
            {
                printf("Login of user %s successful\n", username);
                sprintf(reply, "Login of user %s successful", username);
            }
            else 
            {
                printf("Login of user %s failed\n", username);
                sprintf(reply, "Login of user %s failed", username);
            }
        } 
        
        else 
        {
            printf("Invalid request\n");
            sprintf(reply, "Invalid request");
        }

        if((sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&client_addr, len)) < 0)
            handle_error("[-]Error sendto\n");
            
        printf("[+]Reply sent to client %s\n", ipv6_addr);
    }

    close(sockfd);

    return 0;
}

        
