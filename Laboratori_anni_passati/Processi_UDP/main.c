/*
    Scrivere un programma in C che permetta di inviare un messaggio testuale da un processo client ad un processo server tramite una connessione UDP. 
    Il processo client deve inviare il messaggio sulla porta e sull'indirizzo specificati dal processo server.

    Il processo server deve mettersi in ascolto sulla porta specificata e ricevere i messaggi inviati dal client. 
    Una volta ricevuto un messaggio, il server deve stampare il testo del messaggio sulla console.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ADDR "127.0.0.1"

#define PORT 7561
#define BUFFER_SIZE 1024

int pid;

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char*argv[])
{
    pid = fork();
    
    if(pid > 0) 
    {
        puts("\nServer process:\n");

        int sockfd;
        struct sockaddr_in addr;
        char buffer[BUFFER_SIZE];
        socklen_t size;
        ssize_t rf;

        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            handle_error("[-]Socket error\n");
        printf("Server socket created successfully\n");
        
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ADDR, &addr.sin_addr);
        addr.sin_port = htons(PORT);

        if((bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))) < 0)
            handle_error("[-]Bind error\n");
        printf("[+]Binding success\n");

        size = sizeof(addr);
        for(;;)
        {
            if((rf = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &size)) < 0)
                handle_error("[-]Recvfrom error\n");

            buffer[rf] = '\0';
            if(strncmp(buffer, "STOP", strlen("STOP")) == 0) break;
            printf("[+]Received message:\t%s\n", buffer);
        }

        close(sockfd);

        return 0;

    }
    else if(pid == 0)
    {
        puts("\n-------------------------[-]Client process\n");

        int sockfd;
        struct sockaddr_in addr;
        char buffer[BUFFER_SIZE];

        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            handle_error("[-]Socket error\n");
        printf("Client socket created successfully\n");
        
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ADDR, &addr.sin_addr);
        addr.sin_port = htons(PORT);

        FILE* fp;
        if(!(fp = fopen("file.txt", "r")))
            handle_error("Error fopen\n");

        while(fgets(buffer, BUFFER_SIZE, fp))
        {
            if (strlen(buffer) <= 1) continue; 
            if((sendto(sockfd, buffer, strlen(buffer)+1, 0, (struct sockaddr*)&addr, sizeof(addr))) < 0)
                handle_error("[-]Sendto error\n");
        }
        if((sendto(sockfd, "STOP", strlen("STOP")+1, 0, (struct sockaddr*)&addr, sizeof(addr))) < 0)
            handle_error("[-]Sendto error\n");

        printf("[+]Transmission completed\n");

        close(sockfd);
        puts("\n-------------------------[-]Client disconnected\n");

        return 0;
    }
    else 
        handle_error("[-]Error pid\n");
}
