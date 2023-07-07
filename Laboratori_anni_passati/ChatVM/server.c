#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MSG_LEN 20
#define MAX_CLIENT 6
#define PORT 8000

typedef enum 
{
    C, CPP, JAVA, PYTHON, R, MATLAB
} Language;

typedef struct 
{
    int n; 
    char msg[MSG_LEN];
    char language[MSG_LEN]; 
    char IP[16];
}VMClient;

Language toLanguage(char* str) 
{
    if (strcmp(str, "C") == 0)
        return C;
    else if (strcmp(str, "CPP") == 0)
        return CPP;
    else if (strcmp(str, "JAVA") == 0)
        return JAVA;
    else if (strcmp(str, "PYTHON") == 0)
        return PYTHON;
    else if (strcmp(str, "R") == 0)
        return R;
    else if (strcmp(str, "MATLAB") == 0)
        return MATLAB;
}

char* toString(Language lang) 
{
    if (lang == C)
        return "C";
    else if (lang == CPP)
        return "CPP";
    else if (lang == JAVA)
        return "JAVA";
    else if (lang == PYTHON)
        return "PYTHON";
    else if (lang == R)
        return "R";
    else if (lang == MATLAB)
        return "MATLAB";
}

void handle_error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int** getMatrix(int a, int b)
{
    int ** mat = malloc(sizeof(int*) * MAX_CLIENT);
    for(int i=0; i<MAX_CLIENT; i++)
    {
        mat[i] = malloc(sizeof(int) * MAX_CLIENT);
        for(int j=0; j<=i; j++)
        {
            int r = rand()%(b-a+1)+a;
            if(i == j)
                r = 0;
            mat[i][j] = mat[j][i] = r;
        }
    }
    return mat;
}

void printMatrix(int** mat)
{
    printf("Matrix\n\n");
    for(int i=0; i<MAX_CLIENT; i++)
    {
        for(int j=0; j<MAX_CLIENT; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
    printf("\n");
}

void writeFile(char* buffer, char* ip_sender)
{
    FILE* fp;
    if (!(fp = fopen("VMClients.txt", "a+")))
        handle_error("Error fopen\n");

    fseek(fp, 0, SEEK_SET);

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strncmp(line, buffer, strlen(buffer)) == 0)  //se è già presente
        {
            fclose(fp);
            return;  
        }
    }

    fprintf(fp, "%s %s\n", buffer, ip_sender);
    fflush(fp);

    fclose(fp);
}

void readFile(int index, char** ip)
{
    FILE* fp;
    if (!(fp = fopen("VMClients.txt", "r")))
        handle_error("Error fopen\n");

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, fp))
    {
        char* tok = strtok(line, " ");
        if (toLanguage(tok) == index)
        {
            tok = strtok(NULL, "\n");
            *ip = malloc(strlen(tok) + 1);  // Importante
            strncpy(*ip, tok, strlen(tok));  
            break;
        }
    }

    fclose(fp);
}

char** find_closest_languages(VMClient* client, int** distanceMatrix) 
{
    int n = client->n;
    int id = toLanguage(client->language);
    int *row = distanceMatrix[id];
    int index[MAX_CLIENT];

    //Scrivo nell'array degli indici tutti gli indici a meno di id
    int currentIndex = 0;
    for (int i = 0; i < MAX_CLIENT; i++) 
        if (i != id) index[currentIndex++] = i;

    //Bubble sort 
    for (int i = 0; i < currentIndex - 1; i++) 
    {
        for (int j = i + 1; j < currentIndex; j++) 
        {
            if (row[index[j]] < row[index[i]]) 
            {
                int temp = index[i];
                index[i] = index[j];
                index[j] = temp;
            }
        }
    }

    printf("\nI %d linguaggi più vicini a %s sono ", n, client->language);
    for(int i = 0; i < n; i++)
        printf("%s ", toString(index[i]));
    printf("\n\n");
    
    char** closest = malloc(sizeof(char*) * n);
    for(int i = 0; i < n; i++)
        readFile(index[i], &closest[i]);    //legge dal file ed inizializza le stringhe di IP
    
    return closest;
}

int main(int argc, char* argv[])
{
    int sockfd, new_sock, n;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    int** distanceMatrix = getMatrix(10, 255);
    printMatrix(distanceMatrix);

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("Error socket\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr *)&server_addr, len)) < 0)
        handle_error("Error bind\n");

    listen(sockfd, 5);
    printf("Server listening...\n");

    for(;;)
    {
        if((new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &len)) < 0)
            handle_error("Error accept\n");
        printf("\n[+]Client %s connected to port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if(!fork())
        {
            for(;;)
            {
                if((n = recv(new_sock, buffer, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error recv\n");
                writeFile(buffer, inet_ntoa(client_addr.sin_addr));

                if((n = recv(new_sock, buffer, BUFFER_SIZE, 0)) < 0)
                    handle_error("Error recv\n");

                VMClient client;
                memcpy(&client, buffer, sizeof(client));
  
                printf("\nMessage: %s, IP: %s, Port: %d\n", client.msg, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if((send(new_sock, "ACK", BUFFER_SIZE, 0)) < 0)
                    handle_error("Error send\n");
                
                char** closest = find_closest_languages(&client, distanceMatrix);
                for(int i=0; i<client.n; i++)
                {
                    int client_sock;
                    struct sockaddr_in dest_addr;

                    memset(&dest_addr, 0, len);
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_addr.s_addr = inet_addr(closest[i]);
                    dest_addr.sin_port = htons(PORT);

                    if((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
                        handle_error("Error socket\n");

                    sendto(client_sock, client.msg, MSG_LEN, 0, (struct sockaddr*)&dest_addr, len);

                    close(client_sock);
                }
            }
            close(new_sock);
        }
        else close(new_sock);
    }
    close(sockfd);

    return 0;
}