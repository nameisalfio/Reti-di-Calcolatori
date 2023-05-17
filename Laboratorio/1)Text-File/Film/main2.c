#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Film.h"
#include "List.h"

int main() 
{
    FILE* fin;

    if (!(fin = fopen("film.txt", "r"))) 
    {
        perror("Error fin");
        exit(1);
    }

    List l;
    char* str = NULL;
    char* line = NULL;
    size_t nread;
    size_t size = 0;
    int dim = 0;

    while ((nread = getline(&line, &size, fin)) != -1) 
    {
        char* token;
        Film f;

        token = strtok(line, ";");
        f.title = malloc(strlen(token) + 1);
        strcpy(f.title, token);

        token = strtok(NULL, ";");
        f.year = atoi(token);

        token = strtok(NULL, ";");
        f.score = atof(token);
        
        // Add new film
        insertTail(&l, f);
    }

    printList(&l);

    fclose(fin);
    free(line);

    return 0;
}
 
