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

typedef struct
{
    char name[NAME_SIZE];   
    int amount;
    int* ID_list;
}Resource;

void printRequest(Request* request)
{
    printf("Operation: %s\n", request->operation);
    printf("ID: %d\n", request->ID);
    printf("Resource: %s\n\n", request->resource);
}

char* operation1(Request* request)
{
    // RAM:3:4:2
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFFER_SIZE);
    char line[BUFFER_SIZE];

    //  1)Chiedere l’elenco delle risorse disponibili (quindi con quantità >0)
    while(fgets(line, BUFFER_SIZE, fp) )
    {
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        if(resource.amount > 0)
        {
            strcat(buffer, resource.name);
            strcat(buffer, " ");
        }
    }
    fclose(fp);
    return buffer;
}

char* operation2(Request* request)
{
    // RAM:3:4:2
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFFER_SIZE);
    char line[BUFFER_SIZE];

    //  2)Chiedere di prenotare una data risorsa (il server verifica se è ancora disponibile);
    while(fgets(line, BUFFER_SIZE, fp))
    {
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        if(strcmp(request->resource, resource.name) == 0 && resource.amount > 0)
        {
        }
        strcpy(buffer, "Resource booked");
    }
    fclose(fp);
    return buffer;
}

char* operation3(Request* request)
{
    // RAM:3:4:2
    /*
    CONTENUTO DEL FILE DI TESTO --> <nome>:<quantità_residua>:<client_1>:...:<client_n>

        RAM:5
        CPU:4:3
        HARD-DISK:1
        GPU:0
        DRAM:2
    */
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFFER_SIZE);
    char line[BUFFER_SIZE];

    //  3)Chiedere l’elenco delle risorse già prenotate;

    while(fgets(line, BUFFER_SIZE, fp))
    {
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        resource.ID_list = malloc(sizeof(int)*resource.amount);
        
        int n=0;
        char* str;
        while((str = strtok(NULL, ":")))    //numero non noto a priori di client
            resource.ID_list[n++] = atoi(str);  

        for(int i=0; i<n; i++)
        {
            printf("%d", resource.ID_list[i]);
            if(resource.ID_list[i] == request->ID)
            {
                strcat(buffer, resource.name);
                strcat(buffer, " ");
            }
        }
    }
    fclose(fp);
    return buffer;
}

/*
char* operation3(Request* request)
{
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFFER_SIZE);
    char line[BUFFER_SIZE];

    //  3)Chiedere l’elenco delle risorse già prenotate;

    while(fgets(line, BUFFER_SIZE, fp))
    {

        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        resource.ID_list = malloc(sizeof(int)*resource.amount);
        for(int i=0; i<resource.amount-1; i++){
            char str[BUFFER_SIZE];
            sprintf(str, "%s ", strtok(NULL, ":"));
            strcat(buffer, str);
            //resource.ID_list[i] = atoi(strtok(NULL, ":"));  //SEGMENTATION FAULT
        }
        return buffer;
        for(int i=0; i<resource.amount; i++)
        {
            if(resource.ID_list[i] == request->ID)
            {
                strcat(buffer, resource.name);
                strcat(buffer, " ");
            }
        }
    }
    fclose(fp);
    return buffer;
}
*/

char* operation4(Request* request)
{
    // RAM:3:4:2
    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFFER_SIZE);
    char line[BUFFER_SIZE];

    //  4)Liberare una risorsa prenotata in precedenza(il server verifica che era già riservata).

    while(fgets(line, BUFFER_SIZE, fp))
    {

    }
    fclose(fp);
    return buffer;
}

char* handle_operation(Request* request)
{
    if(strcmp(request->operation, "1") == 0)
        return operation1(request);

    if(strcmp(request->operation, "2") == 0)
        return operation2(request);

    if(strcmp(request->operation, "3") == 0)
        return operation3(request);

    if(strcmp(request->operation, "4") == 0)
        return operation4(request);
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if(argc != 2)
        handle_error("Error argc\n");

    memset(&client_addr, 0, len);
    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    if((bind(sockfd, (struct sockaddr*) &server_addr, len)) < 0)
        handle_error("Error bind\n");
    printf("[+]Server listening...\n\n");

    while(true)
    {
        Request request;
        if((n = recvfrom(sockfd, &request, sizeof(Request), 0, (struct sockaddr*) &client_addr, &len)) < 0)
            handle_error("Errore recvfrom\n"); 

        // Elaborazione della richiesta
        strcpy(buffer, handle_operation(&request));
        printf("Outcome: %s\n", buffer);

        if((sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &client_addr, len)) < 0)
            handle_error("Errore sendto\n");
    }
    close(sockfd);

    return 0;
}