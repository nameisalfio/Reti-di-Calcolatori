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

bool LOG(Message* msg, FILE* fp)
{
    rewind(fp);
    char usr[FIELD_SIZE];
    char pass[FIELD_SIZE];
    while(fscanf(fp, " %s %s", usr, pass) == 2)
    {
        if(strcmp(usr, msg->usr) == 0)  //Non ci sono utenti con lo stesso username
            return true;
    }
    return false;
}

bool REG(Message* msg, FILE* fp)
{
    if(LOG(msg, fp))
        return false;

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s %s\n", msg->usr, msg->pass);
    fflush(fp);
    return true;
}

bool DEL(Message* msg, FILE* fp)
{
    if(!LOG(msg, fp))
        return false;

    rewind(fp);
    FILE* fp_tmp;
    if(!(fp_tmp = fopen("Database_tmp.txt", "w")))  handle_error("Error fopen\n");

    char usr[FIELD_SIZE];
    char pass[FIELD_SIZE];
    while(fscanf(fp, " %s %s", usr, pass) == 2)
    {
        if(strcmp(usr, msg->usr) == 0 && strcmp(pass, msg->pass) != 0)
            fprintf(fp_tmp, "%s %s\n", usr, pass);
    }
    fflush(fp_tmp);
    fclose(fp_tmp);
    fclose(fp);

    remove("Database.txt");
    rename("Database_tmp.txt", "Database.txt");
    fp = fp_tmp;
    return true;
}

char* handle_request(Message* msg, FILE* fp)
{
    if(strcasecmp(msg->op, "LOG") == 0)
        return LOG(msg, fp) ? "Login succesful" : "Login failed";

    if(strcasecmp(msg->op,"REG") == 0)
        return REG(msg, fp) ? "Registration succesful" : "Registration failed";

    if(strcasecmp(msg->op, "DEL") == 0)
        return DEL(msg, fp) ? "Deletion succesful" : "Deletion failed";
}

int main(int argc, char** argv)
{
    int sockfd, newsock, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFSIZ];
    Message msg = {0};
    FILE* fp;

    if(argc != 2)   handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  handle_error("Error socket\n");
    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0)  handle_error("Error bind\n");
    listen(sockfd, 5);
    printf("[+]Server listening...\n\n");

    while(true)
    {
        newsock = accept(sockfd, (struct sockaddr*) &client_addr, &len);
        printf("Client %s connected to port %d\n\n", inet_ntoa(client_addr.sin_addr), atoi(argv[1]));
        if(fork())
        {
            close(sockfd);

            if(!(fp = fopen("Database.txt", "a+")))  handle_error("Error fopen\n");
            while(true)
            {
                if((n = recv(newsock, &msg, sizeof(msg), 0)) < 0)  handle_error("Error recv\n");
                if(n == 0)
                {
                    printf("Client %s disconnected from port %d\n\n", inet_ntoa(client_addr.sin_addr), atoi(argv[1]));
                    exit(EXIT_SUCCESS);
                }
                strcpy(buffer, handle_request(&msg, fp));
                printf("Outcome: %s\n\n", buffer);
                if((send(newsock, buffer, BUFSIZ, 0)) < 0)  handle_error("Error send\n");
            }
            close(newsock);
            fclose(fp);
        }
        else
        {
            close(newsock);
        }
    }

    return 0;
}