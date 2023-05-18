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
    if((fp = fopen("secret.txt", "a+")) < 0)
        handle_error("Error fopen\n");

    fprintf(fp, "%s\n", recvline);

    fclose(fp);
}

char* GET_MSG(char* recvline, char* ip_sender)
{
    FILE* fp;
    if((fp = fopen("secret.txt", "r")) == NULL) {
        handle_error("Error fopen\n");
    }

    //C1 GET_SECRET C4

    char* usr_sender = strtok(recvline, " ");
    char* usr_client = strtok(NULL, " ");
    usr_client = strtok(NULL, "\n");
    char* msg = malloc(MSG_LEN * sizeof(char));

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, fp)) 
    {
        // C2 Secret message : 192.168.1.9-C1, 192.168.56.104-C2, 192.168.1.9-C1, 192.168.56.104-C2

        char* ID = strtok(line, " ");
        printf("ID = %s, Usr = %s\n", ID, usr_client);
        if(strcmp(ID, usr_client) == 0)
        {
            msg = strtok(line, " : ");
            char* ip_file = strtok(NULL, "-");
            char* usr_file = strtok(NULL, ", ");

            while (ip_file && usr_file) 
            {
                printf("Ip: %s %s\tUsr: %s %s\n", ip_sender, ip_file, usr_sender, usr_file);
                if (strcmp(ip_sender, ip_file) == 0 && strcmp(usr_sender, usr_file) == 0) 
                {
                    fclose(fp);
                    return msg;
                }
                ip_file = strtok(NULL, "-");
                usr_file = strtok(NULL, ", ");
            }
        }
    }
    sprintf(msg, "%s", "Error authentication");
    fclose(fp);
    return msg;
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

                if((send(new_sock, recvline, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error send\n");
                PUT_MSG(recvline);  //scrivo nel file

                if((n = recv(new_sock, recvline, BUFFER_SIZE, 0)) < 0) 
                    handle_error("Error recv\n");
                recvline[n] = 0;
                printf("\nMessaggio: %s | IP: %s, Port: %d\n", recvline, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                sprintf(ip_sender, "%s", inet_ntoa(client_addr.sin_addr));  //estraggo l'ip del sender

                strcpy(sendline, GET_MSG(recvline, ip_sender));
                if((send(new_sock, sendline, BUFFER_SIZE, 0)) < 0)    //invio il messaggio o un errore
                    handle_error("Error send\n");
            }   
            close(new_sock);     
        }
        else close(new_sock);

    }
    close(sockfd);
    return 0;
}