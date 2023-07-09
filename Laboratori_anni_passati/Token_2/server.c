#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

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

bool Login(Message* msg)
{
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r"))) handle("fopen");

    char line[BUFSIZ];
    while(fgets(line, BUFSIZ, fp))
    {
        if(!strcmp(msg->usr, strtok(line, " ")))
        {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

char* Register(Message* msg)
{
    if(Login(msg)) return "Register failed";

    char* outcome = malloc(BUFSIZ);
    srand(time(NULL));
    FILE* fp;
    if(!(fp = fopen("Database.txt", "a+"))) handle("fopen");
    int token = rand();
    fprintf(fp, "%s %s %d %d\n", msg->usr, msg->pass, msg->port, token);
    fflush(fp);
    fclose(fp);
    sprintf(outcome, "%d", token);
    return outcome;
}

void SendAll(Message* msg)
{
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r"))) handle("fopen");

    char usr[USR_SIZE], pass[INET6_ADDRSTRLEN];
    int port, token, remote_sock;
    struct sockaddr_in6 remote_addr;
    socklen_t len = sizeof (struct sockaddr_in6);

    memset(&remote_addr, 0, len);  
    remote_addr.sin6_family = AF_INET6;
    if((remote_sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) handle("socket");

    char* msg_tmp = strtok(msg->msg, " ");
    msg_tmp = strtok(NULL, " ");
    sprintf(msg->msg, "%s %s", msg->usr, msg_tmp);
    while(fscanf(fp, " %s %s %d %d", usr, pass, &port, &token) == 4)
    {
        if(strcmp(msg->usr, usr))
        {
            inet_pton(AF_INET6, pass, &remote_addr.sin6_addr);
            remote_addr.sin6_port = htons(port);
            if((sendto(remote_sock, msg->msg, BUFSIZ, 0, (struct sockaddr*)&remote_addr, len)) < 0) handle("sendto");
        }
    }
    close(remote_sock);
    fclose(fp);
}

int main(int argc, char* argv[])
{
    int sock, n;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFSIZ];
    Message msg;

    if(argc != 2) handle("argc");

    memset(&server_addr, 0, len);
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::", &server_addr.sin6_addr);
    server_addr.sin6_port = htons(atoi(argv[1]));

    if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) handle("socket");
    if((bind(sock, (struct sockaddr*)&server_addr, len)) < 0) handle("");

    printf("[+]Server listening...\n\n");
    while(true)
    {
        if((n = recvfrom(sock, &msg, sizeof(Message), 0, (struct sockaddr*)&client_addr, &len)) < 0) handle("recvfrom"); // Usr Port
        inet_ntop(AF_INET6, &client_addr.sin6_addr, msg.pass, INET6_ADDRSTRLEN); // Pass

        if(!strcmp(msg.msg, ""))
        {
            strcpy(buffer, Register(&msg));
            printf("Outcome: %s\n", buffer);
            if((sendto(sock, buffer, BUFSIZ, 0, (struct sockaddr*)&client_addr, len)) < 0) handle("sendto");
        }
        if(strcmp(msg.msg, "")) SendAll(&msg);
    }
    close(sock);

    return 0;
}