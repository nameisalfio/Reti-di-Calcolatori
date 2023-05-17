//CLIENT

//  Sender e receiver UDP. 
//  Il sender spedisce il proprio orario (data ed ora) al receiver tramite un pacchetto UDP. Il receiver stampa a video le informazioni ricevute.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    strcat(msg, "\n");
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char*argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    time_t t;
    struct tm* tm_info;
    char buffer[BUFFER_SIZE];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("[-]Error socket");
    printf("[+]Socket created successfully\n");

    if(argc < 3)
        handle_error("Too few argument at main function");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(addr.sin_addr));
    addr.sin_port = htons(atoi(argv[2]));

    // Generazione dell'orario corrente
    t = time(NULL);
    tm_info = localtime(&t);
 
    // Formattazione dell'orario corrente come stringa
    strftime(buffer, BUFFER_SIZE, "%d-%m-%Y %H:%M:%S", tm_info);

    if((sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*) &addr, sizeof(addr)))< 0)
        handle_error("[-]Error sendto");
    printf("[+]Transmission completed\n");

    close(sockfd);


    return 0;
}