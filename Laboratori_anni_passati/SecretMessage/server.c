#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MSG_LEN 50
#define MAX_CLIENT 5

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void PUT_MSG(char* recvline)
{
    FILE* fp;
    if((fp = fopen("secret.txt", "w")) < 0)
        handle_error("Error fopen\n");

    fprintf(fp, "%s\n", recvline); //  C1 MSG 192.168.1.9-C1  .... 192.168.56.104-C2

    fclose(fp);
}

char* GET(char* recvline, char* ip_sender)
{
    FILE* fp;
    if((fp = fopen("secret.txt", "r")) < 0)
        handle_error("Error fopen\n");

    // GET C1 C3

    char* dup = strdup(recvline);
    char* usr_sender = strtok(dup, " ");
    usr_sender = strtok(NULL, " ");
    char* id = strtok(NULL, "\n");

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, fp))
    {
        dup = strdup(line);
        char* str = strtok(dup, " ");
        if(strcmp(id, str) == 0)
        {        
            //C1 SECRET_MESSAGE1 192.168.1.9-C1 193.67.9.3-C4 192.168.56.104-C2

            char* msg = strtok(line, " ");
            msg = strtok(NULL, " ");
            char* ip_client = strtok(NULL, "-");
            char* usr_client = strtok(NULL, " ");
            while(ip_client && usr_client)
            {
                if(strncmp(ip_client, ip_sender, strlen(ip_sender)) == 0 && strncmp(usr_client, usr_sender, strlen(usr_sender)) == 0)
                {
                    fclose(fp);
                    return msg;
                }
                ip_client = strtok(NULL, "-");
                usr_client = strtok(NULL, " ");
            }
        }
    }
    return "ERROR_AUTHENTICATION";
}

char* CHANGE(char *recvline, char * ip_sender)
{
    FILE* fp;
    if(!(fp = fopen("secret.txt", "r"))) 
        handle_error("Error fopen\n");

    // GET C1

    fclose(fp);
    return " ";
}

int main(int argc, char* argv[])
{
    int sockfd, new_sock, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];
    char ip_sender[BUFFER_SIZE];

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    bzero(sendline, BUFFER_SIZE);
    bzero(recvline, BUFFER_SIZE);

    if((bind(sockfd, (struct sockaddr*)&server_addr, len)) < 0)
        handle_error("Error bind\n");

    listen(sockfd, 5);
    printf("Server listening...\n");

    for(;;)
    { 
        if((new_sock = accept(sockfd, (struct sockaddr*)&client_addr, &len)) < 0)
            handle_error("Error accept\n");
        printf("\n[+]Client connected IP: %s, Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if(!fork())
        { 
            if((n = recv(new_sock, recvline, BUFFER_SIZE, 0)) < 0)
                handle_error("Error recv\n");
            else if(n == 0)
            {
                printf("\n[-]Client disconnected IP: %s, Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }
            recvline[n] = 0;
            printf("\nMessaggio: %s | IP: %s, Port: %d\n", recvline, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            PUT_MSG(recvline);
            for(;;)
            { 
                if((n = recv(new_sock, recvline, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error recv\n");
                else if(n == 0)
                {
                    printf("\n[-]Client disconnected IP: %s, Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }
                recvline[n] = 0;
                printf("\nMessaggio: %s | IP: %s, Port: %d\n", recvline, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                sprintf(ip_sender, "%s", inet_ntoa(client_addr.sin_addr));

                if(strncmp(recvline, "GET", strlen("GET")) == 0)    // GET C1 C3
                    strcpy(sendline, GET(recvline, ip_sender));

                else if(strncmp(recvline, "CHANGE", strlen("CHANGE")) == 0)
                    strcpy(sendline, CHANGE(recvline, ip_sender));

                else    
                    strcpy(sendline, "Invalid request. Enter an authentication request...");

                if((send(new_sock, sendline, BUFFER_SIZE, 0)) < 0)   
                    handle_error("Error send\n");
            }   
            close(new_sock);     
        }
        else close(new_sock);

    }
    close(sockfd);
    return 0;
}