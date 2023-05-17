#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENT 5

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, new_sock, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];

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

                printf("\nMessaggio: %s, IP: %s, Port: %d\n", recvline, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if((send(new_sock, recvline, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error send\n");
            }   
            close(new_sock);     
        }
        else close(new_sock);

    }
    close(sockfd);

    return 0;
}