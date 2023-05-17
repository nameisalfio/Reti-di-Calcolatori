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
    int sockfd, n;
    struct sockaddr_in dest_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&dest_addr, 0, len);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
    dest_addr.sin_port = htons(atoi(argv[2]));

    if((connect(sockfd, (struct sockaddr *)&dest_addr, len)) < 0)
        handle_error("Error connect\n");
    printf("[+]Client connected succesful\n");

    for(;;)
    {
        printf("\nEnter a request:\n");
        fgets(sendline, BUFFER_SIZE, stdin);

        sendline[strcspn(sendline, "\n")] = 0; // Imposta il carattere alla posizione len su null

        if(strncmp(sendline, "STOP", strlen("STOP")) == 0) break;

        if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error send\n");
        
        if((n = recv(sockfd, recvline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error recv\n");
        recvline[n] = 0;

        printf("Reply: %s, IP: %s, Port: %d\n", recvline, inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));
    }

    close(sockfd);
    
    return 0;
} 