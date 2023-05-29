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

typedef struct 
{
    char operation[BUFFER_SIZE];
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
}Message;

void printMessage(Message* msg)
{
    printf("\tOperation : %s\n", msg->operation);
    printf("\tUsername : %s\n", msg->username);
    printf("\tPassword : %s\n\n", msg->password);
}

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool LOG(Message* msg, FILE* fp)
{
    rewind(fp);
    char line[BUFFER_SIZE];
    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* l_username = strtok(line, " ");
        char* l_password = strtok(NULL, "\n");
        if (strcmp(l_username, msg->username) == 0 && (strcmp(l_password, msg->password) == 0))
            return true;
    }
    return false;
}

bool REG(Message* msg, FILE* fp)
{
    if (LOG(msg, fp))
        return false;

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s %s\n", msg->username, msg->password);
    fflush(fp);
    return true;
}

bool DEL(Message* msg, FILE* fp)
{
    if(!LOG(msg, fp))
        return false;

    rewind(fp);
    fclose(fp);  // Chiudi il file pointer fp

    FILE* fp_tmp;
    if(!(fp_tmp = fopen("Database_tmp.txt", "w")))
        handle_error("Error fopen\n");

    char line[BUFFER_SIZE];
    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* l_username = strtok(line, " ");
        char* l_password = strtok(NULL, "\n");
        if(strcmp(l_username, msg->username) != 0 || strcmp(l_password, msg->password) != 0)
            fprintf(fp_tmp, "%s %s\n", l_username, l_password);
    }
    fflush(fp_tmp);
    fclose(fp_tmp);

    remove("Database.txt");
    rename("Database_tmp.txt", "Database.txt");
    fp = fp_tmp; 
    return true; 
}

char* handle_request(Message* msg, FILE* fp)
{
    if(strcasecmp(msg->operation, "REG") == 0)
        return REG(msg, fp) ? "Register succesful" : "Register failed";

    if(strcasecmp(msg->operation, "LOG") == 0)
        return LOG(msg, fp) ? "Login succesful" : "Login failed";

    if(strcasecmp(msg->operation, "DEL") == 0) 
        return DEL(msg, fp) ? "Delete succesful" : "Delete failed";
}

int main(int argc, char* argv[])
{
    int sockfd, n, port_client;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];

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

        printf("Message received\n\n");
        printMessage(&msg);

        strcpy(buffer, handle_request(&msg, fp));
        printf("Outcome operation: %s\n\n", buffer);
        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr* )&client_addr, len)) < 0)
            handle_error("Error sendto\n");
    }
    fclose(fp);
    close(sockfd);

    return 0;
}