#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    if(argc < 3)
    {
        perror("Too few arguments to main function\n");
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("[-]Error socket\n");
        exit(1);
    }
    printf("[+]Client's socket created successfully\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);    //addr.sin_addr.s_addr = htonl(atoi(argv[1]));
    addr.sin_port = htons(atoi(argv[2]));

    FILE* fp;
    if(!(fp = fopen("data.txt", "r")))
    {
        perror("[-]Error fopen\n");
        exit(1);
    }

    socklen_t size = sizeof(addr);
    while(fgets(buffer, BUFFER_SIZE, fp))
    {
        if((sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, size)) < 0)
        {
            perror("[-]Error sendto\n");
            exit(1);
        }
    }
    if((sendto(sockfd, "STOP", strlen("STOP"), 0, (struct sockaddr *)&addr, size)) < 0)
    {
        perror("[-]Error sendto\n");
        exit(1);
    }

    printf("[+]Transmission completed successfully\n");

    close(sockfd); 
}