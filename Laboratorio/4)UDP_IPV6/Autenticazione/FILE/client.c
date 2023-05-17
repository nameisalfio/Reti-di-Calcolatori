#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in6 dest_addr;
    socklen_t len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];
    char ipv6_addr[INET6_ADDRSTRLEN]; 
    size_t n;

    if(argc < 3)
        handle_error("Too few argument to main function\n");

    if((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created\n");

    memset(&dest_addr, 0, len);
    dest_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, argv[1], &(dest_addr.sin6_addr));
    dest_addr.sin6_port = htons(atoi(argv[2]));

    FILE* fp;
    if (!(fp = fopen("requests.txt", "r")))
        handle_error("Error fopen\n");

    printf("\nStarting stransmission\n\n");
    while(fgets(buffer, BUFFER_SIZE, fp))
    {
        if((sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&dest_addr, len)) < 0)
            handle_error("[-]Error sendto\n");
        
        if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&dest_addr, &len)) < 0)
            handle_error("[-]Error recvfrom\n");
        buffer[n] = 0;
        
        inet_ntop(AF_INET6,&(dest_addr.sin6_addr),ipv6_addr, INET6_ADDRSTRLEN);
        printf("Reply: %s\t IP: %s\t Port %d\n\n", buffer, ipv6_addr, ntohs(dest_addr.sin6_port));
    }

    printf("[+]Transmission completed\n");
    close(sockfd);
    fclose(fp);

    return 0;
}