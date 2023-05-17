#include <stdio.h>

int main() 
{
    FILE* fin;

    if( !(fin = fopen("file.txt", "r")) )
    {
        perror("Error fin");
        exit(1);
    }

    char* str = NULL;
    size_t len = 0;

// La variabile len viene inizializzata a 0 in modo che getline() possa allocare dinamicamente la memoria necessaria per la stringa letta

    while (getline(&str, &len, fin) != -1)
    {
        printf("%s", str);
    }

    fclose(fin);

    return 0;
}
