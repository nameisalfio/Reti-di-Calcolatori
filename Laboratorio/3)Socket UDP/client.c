#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
    socket()
    sendto()
    close()
*/

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in dest_addr;
    char buffer[100];

    if(argc < 3)
    {
        perror("Error main argument\n");
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) //UDP
    {
		perror("[-]Client socket error\n");
		exit(1);
	}
	printf("Client socket created\n");

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));
    dest_addr.sin_port = htons(atoi(argv[2]));
    //converte l'indirizzo IP argv[1] in una rappresentazione binaria che viene 
    //poi memorizzata in sin_addr della struttura dest_addr

    printf("Put message : \n");
    scanf("%s", buffer);

    sendto(sockfd, buffer, sizeof(buffer)+1, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    close(sockfd);

    return 0;
}
