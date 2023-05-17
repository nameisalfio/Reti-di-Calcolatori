#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char*argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("[-]Client socket error\n");
        exit(1);
    }
    printf("[+]Client socket created successfully\n");

    if(argc < 3)
    {
        perror("[-]Too few arguments to main function\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(addr.sin_addr));      //addr.sin_addr.s_addr = htonl(atoi(argv[1]));
    addr.sin_port = htons(atoi(argv[2]));

    FILE *fp;
    if (!(fp = fopen("data.txt", "r"))) 
    {
        perror("[-]Error in opening file");
        exit(1);
    }

    while (fgets(buffer, BUFFER_SIZE, fp)) 
    {
        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
        {
            perror("[-]Error in sending data");
            exit(1);
        }
    }
    if (sendto(sockfd, "END_OF_TRANSMISSION", strlen("END_OF_TRANSMISSION"), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
    {
        perror("[-]Error in sending data\n");
        exit(1);
    }

    printf("[+]Transmission completed\n");
    close(sockfd);

    return 0;
}

/*
Mario Rossi M 1997
Giovanni Todaro M 2002
Peppino Impastato M 1000
Mario Rossi M 1997
Giovanni Todaro M 2002
Peppino Impastato M 1000
Mario Rossi M 1997
Giovanni Todaro M 2002
Peppino Impastato M 1000
Mario Rossi M 1997
Giovanni Todaro M 2002
Peppino Impastato M 1000
Mario Rossi M 1997
Giovanni Todaro M 2002
Peppino Impastato M 1000
*/