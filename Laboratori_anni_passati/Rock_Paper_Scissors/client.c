/*
Realizzare un programma scritto in c (o c++) che simuli una partita a "Carta-Sasso-Forbice" fra i due Client, con il 
Server in mezzo a fare da arbitro. Si assuma che il numero di giocatori sia esattamente 2 e che tutti rispettino il 
protocollo descritto. 

Appena avviato, ogni Client si connette al Server per registrarsi, fornendo anche le informazioni che il Server userà 
per comunicare durante la partita (ip, porta).

Quando entrambi i Client si sono registrati, il Server avvia la partita, impostando il numero di vite di entrambi i 
giocatori ad un valore stabilito. Inizia quindi ad interpellare a turno i due giocatori, chiedendo la loro mossa. 

Quando entrambi hanno giocato, il Server comunica il risultato della partita a entrambi i giocatori. Se non si è verificato
un pareggio, toglie una vita a quello che ha perso e ricomincia il ciclo.
Quando un giocatore rimane senza vite, il Server comunica il vincitore ad entrambi e termina la partita.
*/

//CLIENT_1

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define handle(msg)\
do{\
    fprintf(stderr, "[%d] error ", __LINE__);\
    perror(msg);\
    exit(EXIT_FAILURE);\
}while(false);

#define SCREEN "c -> CARTA\nf -> FORBICE\ns -> SASSO"

typedef enum {CARTA = 'c', FORBICE = 'f', SASSO = 's'} Move;

char* move_to_string(Move move)
{
    switch (move)
    {
    case 'c':
        return "carta";
        break;
    case 'f':
        return "forbice";
        break;
    case 's':
        return "sasso";
    }
}

int main(int argc, char* argv[])
{
    int sockfd, n;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFSIZ];
    Move move;
    
    if(argc != 3) handle("argc");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) handle("socket");

    if((connect(sockfd, (struct sockaddr*)&server_addr, len)) < 0) handle("connect");

    printf("Enter username: ");
    fgets(buffer, BUFSIZ, stdin);
    buffer[strcspn(buffer, "\n") ]= '\0';
    
    // Register

    if((send(sockfd, buffer, BUFSIZ, 0)) < 0) handle("send");
    if((n = recv(sockfd, buffer, BUFSIZ, 0)) < 0) handle("recv");
    buffer[n] = 0;
    printf("S: %s\n", buffer);
    
    //Wait the other one
    printf("\n...Wait for start...\n");
    if((n = recv(sockfd, buffer, BUFSIZ, 0)) < 0) handle("recv");
    buffer[n] = 0;
    printf("%s\n\n%s\n", buffer, SCREEN);

    // Start match
    while(true)
    {        
        do
        {
            printf("\nEnter move: ");
            move = getchar();
            while(getchar() != '\n'){}
        }while(move != 'c' && move != 'f' && move != 's');
        printf("Move: %s\n", move_to_string(move));

        if((send(sockfd, &move, sizeof(Move), 0)) < 0) handle("send");
        if((n = recv(sockfd, buffer, BUFSIZ, 0)) < 0) handle("recv");
        buffer[n] = 0;
        printf("%s\n", buffer);

        if(strstr(buffer, "(0 lives)")) break;
    }
    if((n = recv(sockfd, buffer, BUFSIZ, 0)) < 0) handle("recv");
    buffer[n] = 0;
    printf("\n%s\n", buffer);
    close(sockfd);

    return 0;
}