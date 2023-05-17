#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>

/*
    socket()
	bind()
    recvfrom()
    close()
*/

#define IP "127.0.0.1"
#define PORT 8080

int main(int argc, char* argv[])
{	
    int sockfd;
    struct sockaddr_in addr;
    char buffer[100];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) //UDP
    {
		perror("[-]Server socket error\n");
		exit(1);
	}
	printf("[+]Server socket created\n\n");

	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	inet_pton(AF_INET, IP, &(addr.sin_addr));
    addr.sin_port = htons(PORT);

	int n = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(n < 0)
	{
		perror("[-]Binding socket server error\n");
		exit(1);
	}
	printf("[+]Binding socket\n");
	
	listen(sockfd, 5);
	puts("Listening...\n");

	for(;;)
	{
		socklen_t size = sizeof(addr);
		ssize_t nread = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr, &size);

		char client[INET_ADDRSTRLEN];	//16
		inet_ntop(AF_INET, &(addr.sin_addr), client, INET_ADDRSTRLEN);

		printf("Request from client %s:\t%s\n\n", client, buffer);

		// Elabora la richiesta del client
		// ...
		// Invia la risposta al client

		char* response = "OK";
		ssize_t nwrite = sendto(sockfd, response, strlen(response), 0, (struct sockaddr*) &addr, size);
	}

	close(sockfd);

	return 0;
}
