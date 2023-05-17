#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define ADDR "127.0.0.1"    
#define PORT 8085
#define BUFFER_SIZE 1024

typedef struct 
{
    char* nome;     
    char* cognome;
    char sesso;
    int annoNascita;
}Persona;

void printPersona(Persona p)
{
    printf("%s %s %c %d\n", p.nome, p.cognome, p.sesso, p.annoNascita);
}

Persona** createVet()
{
    FILE* fp;
    if(!(fp = fopen("data_filtered.txt", "r")))
    {
        perror("[-]Error fopen\n");
        exit(1);
    }

    Persona** vet = malloc(sizeof(Persona*) * BUFFER_SIZE);
    char nome[BUFFER_SIZE];
    char cognome[BUFFER_SIZE];
    char sesso;
    int annoNascita;

    int i=0;
    while(fscanf(fp, "%s %s %c %d", nome, cognome, &sesso, &annoNascita) == 4)
    {
        vet[i] = malloc(sizeof(Persona*));
        vet[i]->nome = malloc(strlen(nome)+1);
        strcpy(vet[i]->nome, nome);
        vet[i]->cognome = malloc(strlen(cognome)+1);
        strcpy(vet[i]->cognome, cognome);
        vet[i]->sesso = sesso;
        vet[i]->annoNascita = annoNascita;
        i++;
    }
    fclose(fp);
    return vet;
}

void selectionSort(Persona *vet[])
{
    Persona *tmp;
    for (int i = 0; vet[i]; i++) 
    {
        int idx = i;
        for (int j = i+1; vet[j]; j++) 
        {
            if(vet[j]->annoNascita < vet[idx]->annoNascita) idx = j;
        }
        tmp = vet[i]; 
        vet[i] = vet[idx];
        vet[idx] = tmp;
    }
}

int filterName(char* record, char* name)   //Mario Rossi M 1997
{
    char* tmp = malloc(strlen(record)+1);
    strcpy(tmp, record);

    char* copy = strtok(tmp, " ");
    if((strcmp(copy, name)) == 0)return -1;
    return 0;
}

int main(int argc, char * argv[])
{
    int sockfd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("[-]Error socket\n");
        exit(1);
    }
    printf("[+]Server's socket created successfully\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, &addr.sin_addr);    
    addr.sin_port = htons(PORT);

    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Error bind\n");
        exit(1);
    }
    printf("[+]Successfull bind\n\n");
    puts("Server listening ...\n");

    FILE* fp;
    if(!(fp = fopen("data_filtered.txt", "w")))
    {
        perror("[-]Error fopen\n");
        exit(1);
    }

    for(;;)
    {
        socklen_t size = sizeof(addr);
        ssize_t rf;
        if((rf = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &size)) < 0)
        {
            perror("[-]Error recvfrom\n");
            exit(1);
        }
        buffer[rf] = '\0';
        if(strncmp("STOP", buffer, strlen("STOP")) == 0)break;
        if((filterName(buffer, "Mario")) < 0)continue;
        //printf("Request: %s\n", buffer);
        fprintf(fp, "%s", buffer);
    }

    fclose(fp);

    printf("[+]Data received successfully\n\n");

    Persona** vet = createVet();
    selectionSort(vet);
    for (int i = 0; vet[i]; i++) 
    {
        printf("\t%d)", i+1);
        printPersona(*(vet[i]));
        printf("\n");
    }

    close(sockfd);
}