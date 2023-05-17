#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    //Server socket 
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);  
    if(server_sock < 0)
    {
        perror("[-]Server socket error");
        exit(1);
    }
    
    //Server address
    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    //Association
    int n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(n < 0)
    {
        perror("[-]Binding server error");
        exit(1);
    }
    printf("[+]Binding server to the port 8080\n");

    listen(server_sock, 5);

    //Client
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_size;      
    char buffer[1024];

    printf("Listening...\n");

    for(;;)
    {
        client_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_size);  //Establishes a connection between a client and a server
        printf("[+]Client connected\n");

        for(;;) 
        {
            bzero(buffer, 1024);

            n = recv(client_sock, buffer, sizeof(buffer), 0);  // Receive the message from the client

            if(n <= 0) break;

            printf("Client: %s\n", buffer);
            bzero(buffer, 1024);

            strcpy(buffer, "HI, I'M SERVER");
            printf("Server: %s\n", buffer);
            send(client_sock, buffer, strlen(buffer), 0);  // Send the response to the client
        }

        close(client_sock);
        printf("[-]Client disconnected\n\n");
        break;
    }

    close(server_sock);

    return 0;
}