#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_error(char* msg)
{
    perror(msg);
    exit(1);
}

bool REG(char* buffer)
{
    FILE* fp;
    if(!(fp = fopen("users.txt", "a+")))
        handle_error("Error fopen\n");

    char str[BUFFER_SIZE];
    strcpy(str, buffer);
        
    char* request = strtok(str, " ");  
    char* usr = strtok(NULL, " ");
    char* pass = strtok(NULL, "\n");
    char line[BUFFER_SIZE];

    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* usr_tmp = strtok(line, " "); 
        usr_tmp = strtok(NULL, " ");  
        char* pass_tmp = strtok(NULL, " ");
        pass_tmp = strtok(NULL, "\n");

        if(strcmp(usr, usr_tmp) == 0 && strcmp(pass, pass_tmp) == 0)
        {
            printf("Registration of user failed\n");
            return false;
        }
    }

    fprintf(fp,"Username: %s Password: %s\n", usr, pass);
    fflush(fp);
    fclose(fp);
    printf("Registration of user successful\n");
    return true;
}

bool LOG(char* buffer)
{
    FILE* fp;
    if(!(fp = fopen("users.txt", "r")))
        handle_error("Error fopen\n");
    
    char str[BUFFER_SIZE];
    strcpy(str, buffer);
    
    char* request = strtok(str, " ");  
    char* usr = strtok(NULL, " ");
    char* pass = strtok(NULL, "\n");
    char line[BUFFER_SIZE];

    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* usr_tmp = strtok(line, " "); 
        usr_tmp = strtok(NULL, " ");  
        char* pass_tmp = strtok(NULL, " ");
        pass_tmp = strtok(NULL, "\n");

        if(strcmp(usr, usr_tmp) == 0 && strcmp(pass, pass_tmp) == 0)
        {
            fclose(fp);
            return true;
        }
    }
    return false;
}

bool RMV(char* buffer)
{
    FILE* fp;
    if (!(fp = fopen("users.txt", "r")))
        handle_error("Error fopen\n");

    FILE* fp_temp;
    if (!(fp_temp = fopen("users_tmp.txt", "w")))
        handle_error("Error fopen\n");
    
    char str[BUFFER_SIZE];
    strcpy(str, buffer);
    char* request = strtok(str, " ");  
    char* usr = strtok(NULL, " ");
    char* pass = strtok(NULL, "\n");
    char line[BUFFER_SIZE];

    bool removed = false;
    while(fgets(line, BUFFER_SIZE, fp))
    {
        char* usr_tmp = strtok(line, " "); 
        usr_tmp = strtok(NULL, " ");  
        char* pass_tmp = strtok(NULL, " ");
        pass_tmp = strtok(NULL, "\n");

        if(strcmp(usr, usr_tmp) == 0 && strcmp(pass, pass_tmp) == 0)
            removed = true;
        else
            fprintf(fp_temp, "Username: %s Password: %s\n", usr_tmp, pass_tmp);
    }
    fclose(fp);
    fclose(fp_temp);

    if (removed) 
    {
        if (remove("users.txt") != 0)    //rimuovo il vecchio file
            handle_error("Error remove\n");

        if (rename("users_tmp.txt", "users.txt") != 0)   //rinomino il nuovo file 
            handle_error("Error rename\n");
        return true;
    } 
    else 
    {
        remove("users_tmp.txt");
        return false;
    }
}

bool CHANGE_USR(char *buffer) 
{
    FILE *fp;
    if (!(fp = fopen("users.txt", "r"))) 
        handle_error("Error fopen\n");

    FILE *fp_tmp;
    if (!(fp_tmp = fopen("users_tmp.txt", "w")))
        handle_error("Error fopen\n");

    char str[BUFFER_SIZE];
    strcpy(str, buffer);
    char *request = strtok(str, " ");
    char *usr = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    char *new_usr = strtok(NULL, "\n");
    char line[BUFFER_SIZE];

    bool password_changed = false;
    while (fgets(line, BUFFER_SIZE, fp)) 
    {
        char line_copy[BUFFER_SIZE];
        strcpy(line_copy, line);

        char *usr_tmp = strtok(line, " ");
        usr_tmp = strtok(NULL, " ");
        char *pass_tmp = strtok(NULL, " ");
        pass_tmp = strtok(NULL, "\n");

        if (strcmp(usr, usr_tmp) == 0 && strcmp(pass, pass_tmp) == 0) 
        {
            password_changed = true;
            fprintf(fp_tmp, "Username: %s Password: %s\n", new_usr, pass);
        } 
        else 
            fputs(line_copy, fp_tmp);
    }

    fclose(fp);
    fclose(fp_tmp);

    if (password_changed) 
    {
        if (remove("users.txt") != 0) 
            handle_error("Error deleting file\n");
        
        if (rename("users_tmp.txt", "users.txt") != 0) 
            handle_error("Error renaming file\n");
        
        printf("Change user of user successful\n");
    } 
    else 
        remove("users_tmp.txt");

    return password_changed;
}

bool CHANGE_PASS(char *buffer) 
{
    FILE *fp;
    if (!(fp = fopen("users.txt", "r"))) 
        handle_error("Error fopen\n");

    FILE *fp_tmp;
    if (!(fp_tmp = fopen("users_tmp.txt", "w")))
        handle_error("Error fopen\n");

    char str[BUFFER_SIZE];
    strcpy(str, buffer);
    char *request = strtok(str, " ");
    char *usr = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    char *new_pass = strtok(NULL, "\n");
    char line[BUFFER_SIZE];

    bool password_changed = false;
    while (fgets(line, BUFFER_SIZE, fp)) 
    {
        char line_copy[BUFFER_SIZE];
        strcpy(line_copy, line);

        char *usr_tmp = strtok(line, " ");
        usr_tmp = strtok(NULL, " ");
        char *pass_tmp = strtok(NULL, " ");
        pass_tmp = strtok(NULL, "\n");

        if (strcmp(usr, usr_tmp) == 0 && strcmp(pass, pass_tmp) == 0) 
        {
            password_changed = true;
            fprintf(fp_tmp, "Username: %s Password: %s\n", usr, new_pass);
        } 
        else 
            fputs(line_copy, fp_tmp);
    }

    fclose(fp);
    fclose(fp_tmp);

    if (password_changed) 
    {
        if (remove("users.txt") != 0) 
            handle_error("Error deleting file\n");
        
        if (rename("users_tmp.txt", "users.txt") != 0) 
            handle_error("Error renaming file\n");
        
        printf("Change password of user successful\n");
    } 
    else 
        remove("users_tmp.txt");

    return password_changed;
}

int main(int argc, char* argv[])
{
    int sockfd, sockfd_client;
    struct sockaddr_in client_addr, server_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    char reply[BUFFER_SIZE];
    int n;

    if(argc != 2)
        handle_error("Error argc\n");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handle_error("[-]Error socket\n");
    printf("[+]Socket created successful\n");

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*)&server_addr, len)) < 0)
        handle_error("Error bind\n");

    printf("Server listening...\n");
    listen(sockfd, 5);

    for(;;) 
    {
        if((sockfd_client = accept(sockfd, (struct sockaddr*)&client_addr, &len)) < 0)
            handle_error("[-]Error accept\n");

        printf("\n[+]Client %s connected to port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        for(;;) 
        {
            if((n = recv(sockfd_client, buffer, sizeof(buffer), 0)) < 0)
                handle_error("[-]Error recv\n");
            buffer[n] = 0;
            
            if(strncmp(buffer, "STOP\n", strlen("STOP\n")) == 0)
            {
                printf("\n[+]Client %s disconnected\n", inet_ntoa(client_addr.sin_addr));
                break;
            }

            else if(strncmp(buffer, "REG", strlen("REG")) == 0)
            {
                if (REG(buffer))
                    sprintf(reply, "Registration of user successul");
                else 
                    sprintf(reply, "Registration of user failed");
            }

            else if(strncmp(buffer, "LOG", strlen("LOG")) == 0)
            {
                if (LOG(buffer))
                {
                    printf("Login of user succesful\n");

                    if((send(sockfd_client, "Login succesful", strlen("Login succesful"), 0)) < 0)
                        handle_error("[-]Error send\n");

                    if((n = recv(sockfd_client, buffer, sizeof(buffer), 0)) < 0)
                        handle_error("[-]Error recv\n");
                    buffer[n] = 0;

                    if(strncmp(buffer, "CHANGE_USR", strlen("CHANGE_USR")) == 0)
                    {
                        if(CHANGE_USR(buffer))
                            sprintf(reply, "Change username of user successful");
                        else
                            sprintf(reply, "Change username of user failed");
                    }

                    else if(strncmp(buffer, "CHANGE_PASS", strlen("CHANGE_PASS")) == 0)
                    {
                        if(CHANGE_PASS(buffer))
                            sprintf(reply, "Change password of user successful");
                        else
                            sprintf(reply, "Change password of user failed");
                    }

                    else if(strncmp(buffer, "RMV", strlen("RMV")) == 0)
                    {
                        if(RMV(buffer))
                            sprintf(reply, "Removal of user successful");
                        else
                            sprintf(reply, "Removal of user failed");
                    }
                    else
                        sprintf(reply, "Invalid request");
                }
                else 
                {
                    printf("Login of user failed\n");
                    sprintf(reply, "Login of user failed");
                }
            }

            else
                sprintf(reply, "Invalid request");

            printf("\nMessage: %s  IP: %s  Port %d\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            if((send(sockfd_client, reply, strlen(reply), 0)) < 0)
                handle_error("[-]Error send\n");
        }
        
        close(sockfd_client);
    }
    
    close(sockfd);

    return 0;

} 
