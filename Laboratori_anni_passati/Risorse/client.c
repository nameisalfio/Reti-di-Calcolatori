/*
    Un client può:

    1)Chiedere l’elenco delle risorse disponibili (quindi con quantità >0)
    2)Chiedere di prenotare una data risorsa (il server verifica se è ancora disponibile);
    3)Chiedere l’elenco delle risorse già prenotate;
    4)Liberare una risorsa prenotata in precedenza(il server verifica che era già riservata).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define NAME_SIZE 20

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct
{
    char operation[BUFFER_SIZE];
    int ID;
    char resource[NAME_SIZE];
} Request;

void printRequest(Request* request)
{
    printf("\nRequest\n-----------\n");
    printf("Operation: %s\n", request->operation);
    printf("ID: %d\n", request->ID);
    printf("Resource: %s\n", request->resource);
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 3)
        handle_error("Error argc\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    while(true)
    {
        printf("Enter a request: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  

        if(strcasecmp(buffer, "exit") == 0) 
        {
            printf("Goodbye\n");
            break;
        }
        else if(strcmp(buffer, "1") == 0 || strcmp(buffer, "2") == 0 || 
                strcmp(buffer, "3") == 0 || strcmp(buffer, "4") == 0)
        {
            Request request;
            char str[NAME_SIZE];

            strcpy(request.operation, buffer);

            printf("\n-> Enter ID: ");   //Richiedo l'ID
            fgets(str, NAME_SIZE, stdin);
            str[strcspn(str, "\n")] = 0;
            request.ID = atoi(str);

            printf("-> Enter a resource: ");    //Richiedo la risorsa
            fgets(str, NAME_SIZE, stdin);
            str[strcspn(str, "\n")] = 0;
            strcpy(request.resource, str);

            if((sendto(sockfd, &request, sizeof(Request), 0, (struct sockaddr*) &server_addr, len)) < 0)
                handle_error("Error sendto\n");

            if((n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &server_addr, &len)) < 0)
                handle_error("Error recvfrom\n");
            buffer[n] = 0;
            printf("\nOutcome: %s\n\n", buffer);
        }
        else
            printf("Invalid request\n\n");        
    }
    close(sockfd);

    return 0;
}