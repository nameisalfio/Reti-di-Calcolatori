#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>

#define NAME_SIZE 20
#define PORT 8000

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef struct
{
    char operation[BUFSIZ];
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

// <nome_risorsa>:<quantità_residua>:<client1>:...:<client2>

char* operation1(Request* request)
{
    //  1)Chiedere l’elenco delle risorse disponibili (quindi con quantità > 0);

    FILE* fp;
    if(!(fp = fopen("Database.txt", "r")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFSIZ);
    char line[BUFSIZ];

    while(fgets(line, BUFSIZ, fp) )
    {
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        if(resource.amount > 0)
        {
            char str[BUFSIZ];
            sprintf(str, "%s(%d) ", resource.name, resource.amount);
            strcat(buffer, str);
        }
    }
    fclose(fp);
    return buffer;
}

char* operation2(Request* request)
{
    // 2)Chiedere di prenotare una data risorsa (il server verifica se è ancora disponibile);

    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");
    
    FILE* fp_tmp;
    if(!(fp_tmp = fopen("Database_tmp.txt", "a+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFSIZ);
    char line[BUFSIZ];
    long line_start;
    while(fgets(line, BUFSIZ, fp))
    {
        char* old_line = strdup(line);
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        resource.ID_list = malloc(sizeof(int)*resource.amount);
        
        int n=0;
        char* str;
        while((str = strtok(NULL, ":")))    //numero non noto a priori di client
            resource.ID_list[n++] = atoi(str);  

        if(strcmp(request->resource, resource.name) == 0)
        {
            if(resource.amount == 0)
            {
                strcpy(buffer, "Resource not available");
                break;
            }

            char new_line[BUFSIZ];
            char str[BUFSIZ];
            resource.amount--;
            sprintf(new_line, "%s:%d", resource.name, resource.amount);
            for(int i=0; i<n; i++)
            {
                sprintf(str, ":%d", resource.ID_list[i]);
                strcat(new_line, str);
            }
            sprintf(str, ":%d", request->ID);
            strcat(new_line, str);

            fprintf(fp_tmp, "%s\n", new_line); 
            sprintf(buffer, "Client %d reserved %s\n", request->ID, request->resource); 
        }
        else
            fprintf(fp_tmp, "%s", old_line);
    }
    fclose(fp);
    fclose(fp_tmp);

    remove("Database.txt");
    rename("Database_tmp.txt", "Database.txt");

    return buffer;
}

char* operation3(Request* request)
{
    //  3)Chiedere l’elenco delle risorse già prenotate;

    FILE* fp;
    if(!(fp = fopen("Database.txt", "r")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFSIZ);
    strcpy(buffer, "\nElenco --> ");
    char line[BUFSIZ];

    while(fgets(line, BUFSIZ, fp))
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
                strcat(buffer, ", ");
            }
        }
    }
    fclose(fp);
    return buffer;
}

char* operation4(Request* request)
{
    //  4)Liberare una risorsa prenotata in precedenza(il server verifica che era già riservata);

    FILE* fp;
    if(!(fp = fopen("Database.txt", "r+")))
        handle_error("Error fopen\n");

    FILE* fp_tmp;
    if(!(fp_tmp = fopen("Database_tmp.txt", "a+")))
        handle_error("Error fopen\n");

    Resource resource;
    char* buffer = malloc(BUFSIZ);
    char line[BUFSIZ];

    while(fgets(line, BUFSIZ, fp))
    {
        char* old_line = strdup(line);
        strcpy(resource.name, strtok(line, ":"));
        resource.amount = atoi(strtok(NULL, ":"));
        resource.ID_list = malloc(sizeof(int)*resource.amount);
        
        int n=0;
        char* str;
        while((str = strtok(NULL, ":")))    //numero non noto a priori di client
            resource.ID_list[n++] = atoi(str);  

        if(strcmp(request->resource, resource.name) == 0)
        {
            bool match = false;
            for(int i=0; i<n; i++)
            {
                if(resource.ID_list[i] == request->ID)  //scorro la lista dei client che detengono la risorsa
                {
                    match = true;
                    char new_line[BUFSIZ];
                    char str[BUFSIZ];
                    resource.amount++;
                    sprintf(new_line, "%s:%d", resource.name, resource.amount);
                    for(int i=0; i<n; i++)
                    {
                        if(resource.ID_list[i] != request->ID)
                        {
                            sprintf(str, ":%d", resource.ID_list[i]);
                            strcat(new_line, str);
                        }
                    }
                    fprintf(fp_tmp, "%s\n", new_line); 
                    sprintf(buffer, "Client %d released %s\n", request->ID, request->resource); 
                }
            }
            if(!match)
                sprintf(buffer, "Client %d doesn't hold %s\n", request->ID, request->resource);
        }
        else
            fprintf(fp_tmp, "%s", old_line);
    }
    fclose(fp);
    fclose(fp_tmp);

    remove("Database.txt");
    rename("Database_tmp.txt", "Database.txt");

    return buffer;
}

void notify(char* outcome, FILE* fp)
{
    rewind(fp);

    int remote_sockfd;
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    memset(&remote_addr, 0, len);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);

    if((remote_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        handle_error("Error socket\n");

    char IPv4[INET_ADDRSTRLEN];
    while (fscanf(fp, "%s", IPv4) == 1)
    {
        inet_pton(AF_INET, IPv4, &remote_addr.sin_addr);
        if((sendto(remote_sockfd, outcome, BUFSIZ, 0, (struct sockaddr*)&remote_addr, len)) < 0)
            handle_error("Error sendto\n");
    }
}

char* handle_operation(Request* request, char* IPv4)
{
    char* outcome = malloc(BUFSIZ);
    char address[INET_ADDRSTRLEN];
    bool found = false;

    FILE* fp;
    if(!(fp = fopen("Address.txt", "a+")))  //tiene traccia degli IP dei client
        handle_error("Error fopen\n");

    while(fscanf(fp, "%s", address) == 1)
    {
        if(strcmp(IPv4, address) == 0)
            found = true;
    }
    if(!found)
        fprintf(fp, "%s\n", IPv4);

    if(strcmp(request->operation, "1") == 0)
    {
        strcpy(outcome, operation1(request));
        fclose(fp);
        return outcome;
    }

    if(strcmp(request->operation, "2") == 0)
    {
        strcpy(outcome, operation2(request));
        notify(outcome, fp);
        fclose(fp);
        return outcome;
    }
    if(strcmp(request->operation, "3") == 0)
    {
        strcpy(outcome, operation3(request));
        fclose(fp);
        return outcome;
    }
    if(strcmp(request->operation, "4") == 0)
    {
        strcpy(outcome, operation4(request));
        notify(outcome, fp);
        fclose(fp);
        return outcome;
    }}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFSIZ];

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

        char IPv4[INET_ADDRSTRLEN];
        strcpy(IPv4, inet_ntoa(client_addr.sin_addr));

        strcpy(buffer, handle_operation(&request, IPv4)); //Elaborazione
        printf("Outcome: %s\n", buffer);

        if((sendto(sockfd, buffer, BUFSIZ, 0, (struct sockaddr*) &client_addr, len)) < 0)
            handle_error("Errore sendto\n");
    }
    close(sockfd);

    return 0;
}