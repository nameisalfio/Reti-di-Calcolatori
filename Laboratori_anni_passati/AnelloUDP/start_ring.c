#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFSIZ];
    char recvline[BUFSIZ];
    char ID;
    char IPv4[INET_ADDRSTRLEN];

    if(argc != 2)   handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)   handle_error("Error socket\n");

    FILE* fp;
    if(!(fp = fopen("Broadcast_addr.txt", "r"))) handle_error("Error fopen\n");

    while(true)
    {
        printf("Enter a message: ");
        fgets(sendline, BUFSIZ, stdin);
        sendline[strlen(sendline)-1] = 0;

        if(strcasecmp(sendline, "EXIT") == 0)
        {
            printf("Goodbye!\n\n");
            break;
        }
        printf("\n");

        while(fscanf(fp, " %c %s", &ID, IPv4) == 2)
        {
            inet_pton(AF_INET, IPv4, &server_addr.sin_addr);
            if((sendto(sockfd, sendline, BUFSIZ, 0, (struct sockaddr*) &server_addr, len)) < 0)   handle_error("Error sendto\n");
            if((n = recvfrom(sockfd, recvline, BUFSIZ, 0, (struct sockaddr*) &server_addr, &len)) < 0)  handle_error("Error recvfrom\n");
            recvline[n] = 0;

            if(strcmp(recvline, "ACK") == 0)
                printf("Reply from client %c -> %s\n", ID, recvline);
            else
                printf("\nError communication with client %c\n", ID);
        }
        printf("\n");
    }

    fclose(fp);
    close(sockfd);

    return 0;
}