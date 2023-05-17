#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, new_sock, n;
    struct sockaddr_in local_addr, remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&local_addr, 0, len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr *)&local_addr, len)) < 0)
        handle_error("Error bind\n");

    listen(sockfd, 5);
    printf("Server listening..\n\n");

    for(;;)
    {
        new_sock = accept(sockfd, (struct sockaddr *)&remote_addr, &len);
        printf("--> Client connected  IP: %s, Port: %d\n\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

        if(!fork())
        {
            close(sockfd);

            for(;;)
            {
                if((n = recv(new_sock, recvline, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error recv\n");
                recvline[n] = 0;

                printf("Messaggio: %s, IP: %s, Port: %d\n\n", recvline, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

                if((send(new_sock, "ACK", 3, 0)) < 0)
                    handle_error("Error send\n");
            }
            close(new_sock);
        }
        else
            close(new_sock);
    }
    close(sockfd);
    
    return 0;
} 