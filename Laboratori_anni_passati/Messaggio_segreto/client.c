#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_HOST

//  C1 Segreto1 : c2,c3
//  C2,C3,C4
//  LOG C1
//  GET C1

/*
    Inizializzazione: il server apre una socket in ascolto su una porta specifica e attende connessioni dai client. 
    Ogni client si connette al server specificando la porta e l'indirizzo IP del server.

    Autenticazione: una volta stabilita la connessione, il client si autentica inviando al server il proprio username. 
    Il server verifica che l'username sia valido e identifica la connessione tramite l'indirizzo IP del client.

    Condivisione segreto: il client invia al server una lista di altri client con cui desidera condividere il segreto, insieme 
    al contenuto del segreto stesso. Il server memorizza queste informazioni in un file di testo.

    Richiesta segreto: un client X si connette al server e richiede il segreto di un client Y specificando l'username di Y. 
    Il server verifica che X sia presente nella lista di client con cui Y ha condiviso il segreto. In caso positivo, il server invia il segreto a X. 
    In caso negativo, il server invia un messaggio di errore.

    Aggiornamento lista: un client può decidere di aggiornare la lista dei client con cui condivide il segreto inviando una nuova lista al server. 
    Il server sostituisce la vecchia lista con quella nuova.
*/

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char sendline[BUFFER_SIZE];
    char recvline[BUFFER_SIZE];
    
    if(argc != 4)   //  IP Port USR
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");
    
    memset(&addr, 0, len);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    addr.sin_port = htons(atoi(argv[2]));

    bzero(sendline, BUFFER_SIZE);
    bzero(recvline, BUFFER_SIZE);

    if((connect(sockfd, (struct sockaddr*)&addr, len)) < 0)
        handle_error("Error connect\n");
    printf("[+]Connection established\n");

    for(;;)
    {
        printf("\nEnter a request:\n");
        fgets(sendline, BUFFER_SIZE, stdin);
        sendline[strcspn(sendline, "\n")] = 0;

        if(strcmp(sendline, "STOP") == 0)break;

        if((send(sockfd, sendline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error send\n");

        if((n = recv(sockfd, recvline, BUFFER_SIZE, 0)) < 0)
            handle_error("Error recv\n");
        recvline[n] = 0;
        printf("Reply: %s, IP:%s, Port:%d\n", recvline, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    close(sockfd);
    return 0;
}