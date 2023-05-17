#include <stdio.h>      // per le funzioni di input/output standard
#include <stdlib.h>     // per le funzioni di allocazione memoria dinamica
#include <unistd.h>     // per le funzioni di system call standard, tra cui "close"
#include <sys/socket.h> // per le funzioni di gestione dei socket
#include <netinet/in.h> // per la definizione dei tipi di dati e delle costanti per gli indirizzi di rete

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);   //utilizzato per ascoltare le connsessioni in entrata
    //socket (domain. type, protocol);  
    
    struct sockaddr_in servaddr;    // è utilizzata per rappresentare un indirizzo IP con il protocollo IPv4.
    /*
        struct sockaddr_in 
        {
            sa_family_t sin_family;     // Famiglia di indirizzi (AF_INET) -- [sa_family_t è un intero a 16 bit]
            in_port_t sin_port;         // Porta del socket
            struct in_addr sin_addr;    // Indirizzo IPv4 del socket
            char sin_zero[8];           // Padding per mantenere la struttura allineata
        };
    */

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //l'indirizzo "INADDR_ANY" rappresenta l'indirizzo IP locale del sistema. 
                                                    //La funzione "htonl" viene utilizzata per convertire l'indirizzo IP da ordine 
                                                    //degli host a ordine di rete, se necessario.
    servaddr.sin_port = htons(8080);                //La funzione htons() viene utilizzata per convertire l'ordine degli byte della 
                                                    //porta 8080 dal formato host all'ordine di rete
    
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));   //associa il socket ad un indirizzo IP e ad una porta specifica
    //bind(socket_descriptor, sockaddr*, lenght of local adress);
    
    listen(sockfd, 5);  //massimo 5 connessioni pendenti sul socket sockfd
    
    int connfd = accept(sockfd, (struct sockaddr *)NULL, NULL); //Utilizzato per comunicare con un client specifico che si è connesso al server.
                                                                //La funzione accept() viene utilizzata per accettare una connessione 
                                                                //in entrata dal socket di ascolto e creare un nuovo socket 
                                                                //Vengono memorizzati i dati della connessione in ingresso nella struct sockaddr
    //accept(socket_in_ascolto, sockaddr*, dim_sockaddr)

    //Una volta stabilita la connessione, è possibile inviare e ricevere dati utilizzando le funzioni "send" e "recv".

    char buffer[1024];
    int n = recv(connfd, buffer, sizeof(buffer), 0);    //usata per ricevere parametri da un socket. Restituisce il numero di byte ricevuti
    //recv(connfd, buffer, sizeof(buffer), flag)
    
    buffer[n] = '\0';
    printf("Received message: %s\n", buffer);
    
    send(connfd, "Hello, client!", 14, 0);  //invia messaggi attraverso un socket
    //send(socket_destinazione, messaggio, dimensione in byte del messaggio, flag)
    
    close(sockfd);
    close(connfd);
    
    return 0;
}

/*
In questo programma sockfd funge da socket del server e connfd funge da socket del client.
Il socket del server viene creato utilizzando la funzione socket(), associato a un indirizzo IP e una porta specifica tramite la funzione bind(), messo 
in ascolto per le connessioni in entrata tramite la funzione listen(). Quando un client richiede una connessione, viene utilizzata la funzione accept() per 
accettare la connessione in entrata, creare un nuovo socket per la comunicazione con il client e restituire il descrittore del socket connfd.
Successivamente, la comunicazione tra il client e il server avviene tramite le funzioni send() e recv(), utilizzate per inviare e ricevere dati attraverso i
rispettivi socket. Infine, i socket vengono chiusi utilizzando la funzione close().
*/