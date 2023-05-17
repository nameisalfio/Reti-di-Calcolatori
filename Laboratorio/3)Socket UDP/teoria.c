/*
    Socket di rete

    $ ip addr -> restituisce la configurazione delle schede di rete e ci permette di sfruttare queste informazioni per 
                 collegarci via ssh

    Una scheda di rete ha un nome che ci permette di identificarla. 
    State UP indica che la scheda di rete è attiva
    troviamo un MAC adress 08:00:27:19:d6:b5
    inet indica un indirizzo ipv4
    inet6 indica indirizzo ipv6 e ha molti elementi in comune con il MAC
    Se non riusciamo a collegarci con l'indirizzo ipv4 , posso sempre utilizzare l'indirizzo ipv6 e dipende dal MAC adress
    Quando clono una macchina devo generare nuovo indirizzi per non creare conflitti.

    Un socket di rete identifica univocamente una connessione di tipo TCP o UDP.
    Un socket richiede 5 informazioni :

    1) indirizzo IP destinatario
    2) porta destinatario
    3) indirizzo IP mittente
    4) porta mittente
    5) tipo di protocollo utilizzato (TCP o UDP)

    Server :
    - socket()
    - bind()
    - recvfrom()    <--
    - sendto()      -->*
    - close()

    Client :
    - socket()
    - sendto()      -->
    - recvfrom()    <--*
    - close

    Con bind() il processo elabora tutti i dati che verranno inviati nella determinata porta specificata. 
    Passeremo la porta come argomenti della funzione main. Questo comando viene utilizzato solo dal lato server.

    La funzione sendto() prenderà come parametro il descrittore di socket dell'host di destinazione.

    Il server contiene una chiamata bloccante recvfrom() che lo mette in ascolto attivo su una determinata porta.

    Utilizziamo delle strutture di supporto :

        struct sockaddr
        {
            u_short sa_family;  //socket adress family, usato per distinguere tra ipv4 o ipv6 (AF_INET opp AF_INET6)
            char sa_data[14];   //socket adress data
        }

    -- AF_INET --

        struct in_addr
        {
            u_int32_t s-addr;
        }

        struct sockaddr_in
        {
            short int sin_family;
            unsigned short int sin_port;
            struct in_addr sin_addr;
            unsigned char sin_zero[8];
        }

    -- AF_INET6 --

        struct sockaddr_in6 
        {
            sa_family_t     sin6_family;    // Famiglia di indirizzo (IPv6)
            in_port_t       sin6_port;      // Numero di porta
            uint32_t        sin6_flowinfo;  // Informazioni di flusso IPv6
            struct in6_addr sin6_addr;      // Indirizzo IPv6
            uint32_t        sin6_scope_id;  // Identificatore di ambito IPv6
        };

    inet_ntop e inet_pton sono due funzioni della libreria di sistema di programmazione di socket in C utilizzate per la conversione degli indirizzi IP 
    in formato testuale (stringhe) e viceversa.

    La funzione inet_ntop (abbreviazione di "internet network to presentation") viene utilizzata per convertire un indirizzo IP binario in formato 
    testuale (stringa). La sua sintassi è la seguente:

    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
    dove:

    - af è la famiglia di indirizzi (AF_INET per IPv4 o AF_INET6 per IPv6);
    - src è il puntatore all'indirizzo IP binario da convertire;
    - dst è il puntatore al buffer di destinazione in cui verrà scritta la stringa risultante;
    - size è la dimensione del buffer di destinazione.

    La funzione inet_pton (abbreviazione di "internet presentation to network") viene utilizzata per convertire un indirizzo IP in formato testuale (stringa) in formato binario. La sua sintassi è la seguente:

    int inet_pton(int af, const char *src, void *dst);
    dove:

    - af è la famiglia di indirizzi (AF_INET per IPv4 o AF_INET6 per IPv6);
    - src è il puntatore alla stringa contenente l'indirizzo IP in formato testuale da convertire;
    - dst è il puntatore alla variabile in cui verrà scritto l'indirizzo IP binario risultante.

    La funzione socket() è una chiamata di sistema utilizzata per creare un nuovo socket di comunicazione. La sua sintassi è la seguente:

--> SOCKET 

    int socket(int domain, int type, int protocol);
    dove:

    - domain specifica la famiglia di indirizzi utilizzata dal socket (ad esempio, AF_INET per IPv4 o AF_INET6 per IPv6);
    - type specifica il tipo di socket (ad esempio, SOCK_STREAM per una connessione di tipo TCP o SOCK_DGRAM per una connessione di tipo UDP);
    - protocol specifica il protocollo utilizzato dal socket (ad esempio, IPPROTO_TCP per TCP o IPPROTO_UDP per UDP).
    
--> BIND

    La funzione bind() è una chiamata di sistema utilizzata per associare un indirizzo IP e un numero di porta ad un socket. La sua sintassi è la seguente:

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    dove:

    - sockfd è il descrittore del socket da associare all'indirizzo IP e al numero di porta specificati;
    - addr è un puntatore ad una struttura sockaddr che contiene l'indirizzo IP e il numero di porta a cui associare il socket;
    - addrlen è la dimensione della struttura sockaddr.

    se la porta è in uso da un altro processo la bind ritorna un codice di errore

--> SENDTO e RECVFROM

    La funzione sendto() viene utilizzata per inviare dati ad un indirizzo specifico tramite un socket. La sua sintassi è la seguente:

    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
    dove:

    sockfd è il descrittore del socket tramite il quale inviare i dati;
    buf è un puntatore al buffer contenente i dati da inviare;
    len è la lunghezza dei dati da inviare;
    flags sono i flag di invio (ad esempio, MSG_DONTWAIT per il non-bloccante);
    dest_addr è un puntatore alla struttura sockaddr contenente l'indirizzo a cui inviare i dati;
    addrlen è la dimensione della struttura sockaddr.
    La funzione restituisce il numero di byte inviati, oppure -1 in caso di errore.

    La funzione recvfrom() viene utilizzata per ricevere dati da un socket provenienti da un indirizzo specifico. La sua sintassi è la seguente:

    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
    dove:

    sockfd è il descrittore del socket dal quale ricevere i dati;
    buf è un puntatore al buffer in cui memorizzare i dati ricevuti;
    len è la lunghezza massima dei dati da ricevere;
    flags sono i flag di ricezione (ad esempio, MSG_DONTWAIT per il non-bloccante);
    src_addr è un puntatore alla struttura sockaddr in cui memorizzare l'indirizzo del mittente;
    addrlen è un puntatore alla dimensione della struttura sockaddr.
    La funzione restituisce il numero di byte ricevuti, oppure -1 in caso di errore.
*/