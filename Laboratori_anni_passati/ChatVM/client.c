#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MSG_LEN 20
#define PORT 8000
#define LANG "C"    //Sostituire con il linguaggio di riferimento della VMClient

typedef struct 
{
    int n; 
    char msg[MSG_LEN];
    char language[MSG_LEN]; 
    char IP[16];
}VMClient;

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    int sockfd, udp_sockfd, n;
    struct sockaddr_in addr, udp_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    if((udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&addr, 0, len);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));

    memset(&udp_addr, 0, len);
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_addr.sin_port = htons(PORT); 

    if((connect(sockfd, (struct sockaddr* )&addr, len)) < 0)
        handle_error("Error connect\n");
    printf("[+]Connection established\n\n");

    if(!fork())
    {
        if((send(sockfd, LANG, BUFFER_SIZE, 0)) < 0)
            handle_error("Error send\n");
        printf("%s\n-------------\n", LANG);

        VMClient client;
        strcpy(client.language, LANG);
        strcpy(client.IP, inet_ntoa(addr.sin_addr));

        do 
        {
            printf("Enter a number between 1 and 6:\n");
            scanf("%d", &client.n);
        } while (client.n < 1 || client.n > 6);

        do
        {
            printf("\nEnter a message:\n");
            scanf("%s", &client.msg);
        } while (strlen(client.msg) >= MSG_LEN);

        if((send(sockfd, &client, sizeof(client), 0)) < 0)
            handle_error("Error send\n");

        if((n = recv(sockfd, buffer, BUFFER_SIZE, 0)) < 0)
            handle_error("Error recv\n");
        buffer[n] = 0;
        printf("\n[+]VMServer Pascal: %s\n", buffer);

        close(sockfd);
    }
    else
    {
        if((bind(udp_sockfd, (struct sockaddr *)&udp_addr, len)) < 0)
            handle_error("Error bind\n");

        if((n = recvfrom(udp_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&udp_addr, &len)) < 0)
            handle_error("Error recvfrom\n");
        buffer[n] = 0;
        printf("\nMessage: %s\n", buffer);

        close(udp_sockfd);
    }

    return 0;
}