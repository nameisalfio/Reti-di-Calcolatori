#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    //Client socket 
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);  
    if(client_sock < 0)
    {
        perror("[-]client socket error");
        exit(1);
    }

    //Adress to connect to
    struct sockaddr_in client_addr;
    memset(&client_addr, '\0', sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(8080);

    //Client connects to the server to request information or services
    connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr));
    printf("Connected to the server\n");

    socklen_t client_size;
    char buffer[1024];
    int n;

    for(;;)
    {
        printf("Enter a message to send to the server: ");  // Read the message from the user
        fgets(buffer, 1024, stdin);

        send(client_sock, buffer, strlen(buffer), 0);  // Send the message to the server
        bzero(buffer, 1024);

        recv(client_sock, buffer, sizeof(buffer), 0);  // Receive the response from the server
        printf("Server: %s\n", buffer);
    }

    close(client_sock);
    printf("Disconnected from the server\n");

    return 0;
}