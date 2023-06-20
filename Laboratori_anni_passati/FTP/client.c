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

int main(int argc, char** argv)
{
    int sockfd, n;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char filename[BUFSIZ], file[BUFSIZ];
    FILE* fp;

    if(argc != 3) handle("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) handle("Error socket\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if((connect(sockfd, (struct sockaddr*) &server_addr, len)) < 0) handle("Error connect\n");
    printf("[+]Client connected\n\n");

    while(true)
    {
        printf("Enter a name of file to download from FTP Server: ");
        fgets(filename, BUFSIZ, stdin);
        filename[strlen(filename)-1] = 0;

        if(strcasecmp(filename, "EXIT") == 0) break;

        if((send(sockfd, filename, BUFSIZ, 0)) < 0) handle("Error send\n");

        if(!(fp = fopen(filename, "w"))) handle("Error fopen\n");

        while((n = recv(sockfd, file, BUFSIZ, 0)) > 0) 
            fwrite(file, sizeof(char), n, fp);
    }
    close(sockfd);
    fclose(fp);

    return 0;
}

