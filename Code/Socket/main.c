#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

//server che ascolta le connessioni dei client e comunica con essi tramite il protocollo TCP.

int main()
{
    //SERVER
    int server = socket(AF_INET, SOCK_STREAM, 0);

    //IP ADRESSS
    struct sockaddr_in ip;
    ip.sin_family = AF_INET;
    ip.sin_addr.s_addr = htonl(INADDR_ANY); //local system's IP
    ip.sin_port = htons(8080);

	if(bind(server, (struct sockaddr*) &ip, sizeof(ip)) < 0)
	{
		fprintf(stderr, "IP association failed\n");
		exit(-1);
	}

    listen(server, 1);
    printf("\nServer listening...\n");

    //CLIENT
    int connfd = accept(server, (struct sockaddr*)NULL, NULL);
    if(connfd < 0)
	{
		fprintf(stderr, "Connection failed\n");
		exit(-1);
	}
	else
		puts("\t---Connection started---");

    send(connfd, "Hello client!\n", 14, 0);

    //COMMUNICATION
    while (true) 
    {
        send(connfd, "Request : \t", 10, 0);

        char buffer[1024];
        int n = recv(connfd, buffer, sizeof(buffer), 0);
        buffer[n] = '\0';

        printf("\nReceived message: %s", buffer);

        if (n <= 0) // Se n è minore o uguale a 0, la connessione è stata chiusa
        {
            printf("\n");
            break;
        }
    }
    
    close(server);
    close(connfd);
    return 0;
}