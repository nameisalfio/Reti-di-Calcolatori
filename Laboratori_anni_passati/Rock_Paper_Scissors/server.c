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

//SERVER

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

#define USR_SIZE 20
#define LIVES_MAX 2

typedef enum {CARTA = 'c', FORBICE = 'f', SASSO = 's'} Move;
typedef enum {PAREGGIO = 0, CLIENT_1 = 1, CLIENT_2 = 2} Winner;

char* Register(char usr[], char ipv4[], int port, int* num_reg)
{
    FILE* fp;
    if(!(fp = fopen("Database.txt", "a+"))) handle("fopen\n");

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s %s %d\n", usr, ipv4, port);
    (*num_reg) ++;    

    fclose(fp);
    return "Register succesful";
}

Winner whowin(Move m1, Move m2)
{
    if (m1 == m2)
        return PAREGGIO;

    if ((m1 == CARTA && m2 == SASSO) || 
        (m1 == SASSO && m2 == FORBICE) || 
        (m1 == FORBICE && m2 == CARTA))
        return CLIENT_1;

    return CLIENT_2;
}

char* winner_to_string(Winner winner)
{
    switch (winner)
    {
    case 1:
        return "CLIENT_1";
        break;
    case 2:
        return "CLIENT_2";
        break;
    default:
        return "Pareggio";
    }
}

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
    int sockfd, newsock, n, port;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFSIZ], ipv4[INET_ADDRSTRLEN], usr[USR_SIZE];
    int num_reg, num_move;
    unsigned c1_lives , c2_lives;
    
    if(argc != 2) handle("argc");

    num_reg = num_move = 0;
    memset(&server_addr, 0, len);

    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) handle("socket");
    if((bind(sockfd, (struct sockaddr*)&server_addr, len)) < 0) handle("bind");
    listen(sockfd, 5);
    printf("[+]Server listening...\n\n");

    // Register client
    int c1_sock, c2_sock;
    while(num_reg < 2)
    {
        if((newsock = accept(sockfd, (struct sockaddr*)&client_addr, &len)) < 0) handle("accept\n");
        printf("[+]New player connected\n");

        if(num_reg == 0)
        {
            c1_sock = newsock;
            if((n = recv(c1_sock, buffer, BUFSIZ, 0)) < 0) handle("recv");
            buffer[n] = 0;

            // <USR> <IP> <PORT>
            strcpy(usr, buffer);
            inet_ntop(AF_INET, &client_addr.sin_addr, ipv4, INET_ADDRSTRLEN);
            port = ntohs(server_addr.sin_port);

            strcpy(buffer, Register(usr, ipv4, port, &num_reg));
            printf("Outcome register: %s, num_reg: %d\n\n", buffer, num_reg);
            if((send(c1_sock, buffer, BUFSIZ, 0)) < 0) handle("send");
        }
        else if(num_reg == 1)
        {
            c2_sock = newsock;
            if((n = recv(c2_sock, buffer, BUFSIZ, 0)) < 0) handle("recv");
            buffer[n] = 0;

            // <USR> <IP> <PORT>
            strcpy(usr, buffer);
            inet_ntop(AF_INET, &client_addr.sin_addr, ipv4, INET_ADDRSTRLEN);
            port = ntohs(client_addr.sin_port);

            strcpy(buffer, Register(usr, ipv4, port, &num_reg));
            printf("Outcome register: %s, num_reg: %d\n", buffer, num_reg);
            if((send(c2_sock, buffer, BUFSIZ, 0)) < 0) handle("send");
        }
    }

    if((send(c1_sock, "START", strlen("START"), 0)) < 0) handle("send");
    if((send(c2_sock, "START", strlen("START"), 0)) < 0) handle("send");

    // Start match
    c1_lives = c2_lives = LIVES_MAX;
    Winner winner;
    while(c1_lives > 0 && c2_lives > 0)
    {
        Move m1, m2;

        if((n = recv(c1_sock, &m1, sizeof(Move), 0)) < 0) handle("recv");
        if((n = recv(c2_sock, &m2, sizeof(Move), 0)) < 0) handle("recv");
        winner = whowin(m1, m2);

        if(winner == CLIENT_1) c2_lives --;
        else if(winner == CLIENT_2) c1_lives --;

        sprintf(buffer, "C1: %s(%d lives)| C2: %s(%d lives), Match: %s", move_to_string(m1), c1_lives, move_to_string(m2), c2_lives, winner_to_string(winner));
        printf("%s\n", buffer);
        if((send(c1_sock, buffer, BUFSIZ, 0)) < 0) handle("send");
        if((send(c2_sock, buffer, BUFSIZ, 0)) < 0) handle("send");
    }

    // End match
    if(c1_lives > 0) sprintf(buffer, "The winner is CLIENT_1");
    else if(c2_lives > 0) sprintf(buffer, "The winner is CLIENT_2");
    printf("\n%s\n", buffer);

    if((send(c1_sock, buffer, BUFSIZ, 0)) < 0) handle("send");
    if((send(c2_sock, buffer, BUFSIZ, 0)) < 0) handle("send");

    close(c1_sock);
    close(c2_sock);
    close(sockfd);

    return 0;
}