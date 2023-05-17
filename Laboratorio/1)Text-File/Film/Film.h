#ifndef FILM_H
#define FILM_H

typedef struct 
{
    char* title;
    int year;
    float score;
} Film;

void printFilm(Film f) 
{
    printf("%s - %d - %f\n", f.title, f.year, f.score);
}

bool equal(Film f1, Film f2)
{
    if(strcmp(f1.title, f2.title) == 0 && f1.year == f2.year && f1.score == f2.score)
        return true;
    return false;
}

#endif