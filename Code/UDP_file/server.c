#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define ADDR "127.0.0.1"
#define PORT 8080
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

void printVet(Persona *vet[])
{
    int i = 0;
    while (vet[i])
    {
        printf("%d)\t", i);
        printPersona(*vet[i]);
        i++;
    }
}

int main(int argc, char* argv[])
{	
    int sockfd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];
    char client[INET_ADDRSTRLEN];	//16

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
		perror("[-]Server socket error\n");
		exit(1);
	}
	printf("[+]Server socket created\n");

	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	inet_pton(AF_INET, ADDR, &(addr.sin_addr));
    addr.sin_port = htons(PORT);

	int n = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(n < 0)
	{
		perror("[-]Error binding to server\n");
		exit(1);
	}
	printf("[+]Binding socket\n\n");
    printf("Server listening...\n\n");

    FILE *fp;
    if (!(fp = fopen("data_received.txt", "w"))) 
    {
        perror("[-]Error in opening file");
        exit(1);
    }

    for (;;) 
    {
        socklen_t len = sizeof(addr);
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &len);
        if (n < 0) 
        {
            perror("[-]Error in receiving data");
            exit(1);
        }
        buffer[n] = '\0';
        if (strncmp("END_OF_TRANSMISSION", buffer, strlen("END_OF_TRANSMISSION")) == 0) break;
        printf("Received message: %s (length: %zu)\n", buffer, strlen(buffer));
        fputs(buffer, fp);
        fflush(fp);
    }

    fclose(fp);   
    fsync(fileno(fp));  // sincronizzo i dati con il file system per assicurarmi che siano salvati correttamente
    printf("\n[+]Data received and saved successfully\n");
    
    FILE *fin;
    if (!(fin = fopen("data_received.txt", "r"))) 
    {
        perror("[-]Error in opening file");
        exit(1);
    }

    Persona* vet[BUFFER_SIZE];
    char nome[BUFFER_SIZE];
    char cognome[BUFFER_SIZE];
    char sesso;
    int annoNascita;    

    int i=0;
    while(fscanf(fin, "%s %s %c %d", nome, cognome, &sesso, &annoNascita) == 4)
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

    puts("\nArray di Persone : \n");
    printVet(vet);

    fclose(fin);
    close(sockfd);
	inet_ntop(AF_INET, &(addr.sin_addr), client, INET_ADDRSTRLEN);
	printf("\nClient %s disconnected....\n", client);

    return 0;
}