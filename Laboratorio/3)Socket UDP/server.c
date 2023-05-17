#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

/*
    socket()
	bind()
    recvfrom()
    close()
*/

#define IP "127.0.0.1"
#define PORT 5000

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
	printf("Server socket created\n");

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
	printf("Binding socket\n");
	
	listen(sockfd, 5);
	printf("Listening...\n");

	socklen_t size = sizeof(addr);
	recvfrom(sockfd, buffer, sizeof(buffer)+1, 0, (struct sockaddr*) &addr, &size);

	printf("Message: %s\n", buffer);

	close(sockfd);
	printf("Server disconnected....\n");

	return 0;
}
