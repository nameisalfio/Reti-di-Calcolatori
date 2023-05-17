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
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 2)
        handle_error("Error argc!\n");

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");

    int broadcastEnable = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    
    memset(&remote_addr, 0, len);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &(remote_addr.sin_addr)); //setto l'indirizzo da qui 
    remote_addr.sin_port = htons(atoi(argv[1]));

    for(;;)
    {
        printf("Enter a request:\n");
        fgets(buffer, BUFFER_SIZE, stdin);

        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&remote_addr, len)) < 0)
            handle_error("Error sendto\n");

        printf("Message: %s IP: %s Port: %d\n", buffer, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
    }

    close(sockfd);

    return 0;
    
}