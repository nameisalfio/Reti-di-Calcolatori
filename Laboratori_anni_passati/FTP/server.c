#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

void handle(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void handle_request(int sockfd)
{
    int n;
    char filename[BUFSIZ], file[BUFSIZ];
    FILE* fp;

    if((recv(sockfd, filename, BUFSIZ, 0)) < 0) handle("Error recv\n");
    printf("Request of download: %s\n", filename);

    if(!(fp = fopen(filename, "r"))) handle("Error fopen\n");

    while((n = fread(file, sizeof(char), BUFSIZ, fp)) > 0)
        if((send(sockfd, file, n, 0)) < 0) handle("Error send\n");
}

int main(int argc, char** argv)
{
    int sockfd, newsock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    if(argc != 2) handle("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) handle("Error socket\n");

    memset(&client_addr, 0, len);
    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0) handle("Error bind\n");
    
    listen(sockfd, 5);
    printf("[+]Server listening\n\n");

    while(newsock = accept(sockfd, (struct sockaddr*)&client_addr, &len))
    {
        printf("[+]Connection accepted\n");
        if(!fork())
        {
            close(sockfd);
            handle_request(newsock);
            close(newsock);
            exit(EXIT_SUCCESS);
        }
        else
            close(newsock);
    }

    return 0;
}


