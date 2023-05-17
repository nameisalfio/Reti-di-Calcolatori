#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* title;
    int year;
    float score;
} Film;

void printFilm(Film f) {
    printf("%s - %d - %f\n", f.title, f.year, f.score);
}

int main() {
    FILE* fin;

    if (!(fin = fopen("film.txt", "r"))) 
    {
        perror("Error fin");
        exit(1);
    }

    Film* vet = malloc(sizeof(Film));
    int DIM = 0;

    char* str = NULL;
    char* line = NULL;
    size_t nread;
    size_t size = 0;

    while ((nread = getline(&line, &size, fin)) != -1) {
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
        DIM++;
        vet = realloc(vet, sizeof(Film) * DIM);
        vet[DIM - 1] = f;

    }

    for (int i = 0; i < DIM; i++) 
    {
        printf("%d)", i);
        printFilm(vet[i]);
    }

    fclose(fin);
    free(line);
    free(vet);

    return 0;
}
