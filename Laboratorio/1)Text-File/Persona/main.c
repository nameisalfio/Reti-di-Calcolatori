#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DIM 100

/*
    ARGOMENTI DEL CORSO DI LABORATORIO

    - Sistemi di virtualizzazione : aspetti teorici e implementazioni
    - Creazione di una VM linux based
    - Configurazione di una VM e collegamento in una LAN privata
    - Configurazione di una interfaccia di rete con IPV4 e IPV6
    - Configurazione delle tabelle di routing
    - Configurazione di una rete LAN differenti connesse da router
    - Uso dei socket in C
    - Esempio di un sistema Client-Server con UDP
    - Esempio di un sistema multiserver con TCP
*/

typedef struct
{
    char *nome;
    char *cognome;
    char sesso;
    int annoNascita;
} Persona;

void printPersona(Persona p)
{
    printf("%s %s %c %d\n", p.nome, p.cognome, p.sesso, p.annoNascita);
}

void printVet(Persona *vet[])
{
    int i = 0;
    while (vet[i])
    {
        printf("%d)\t", i);
        printPersona(*vet[i]);
        i++;
    }
}

void find_max_min(Persona *vet[], Persona *min, Persona *max)
{
    *min = *max = *vet[0];
    int i = 0;
    while (vet[i])
    {
        if (vet[i]->annoNascita > min->annoNascita)
            *min = *vet[i];
        if (vet[i]->annoNascita < max->annoNascita)
            *max = *vet[i];
        i++;
    }
}

int main()
{
    FILE *fin;
    FILE *fout;

    if (!(fin = fopen("input.txt", "r")))
    {
        perror("Error fin");
        exit(1);
    }

    if (!(fout = fopen("output.txt", "w")))
    {
        perror("Error fout");
        exit(1);
    }

    Persona *vet[DIM];
    char nome[DIM];
    char cognome[DIM];
    char sesso;
    int annoNascita;

    int i = 0;
    while (fscanf(fin, "%s %s %c %i", nome, cognome, &sesso, &annoNascita) == 4 && i < DIM)
    {
        vet[i] = malloc(sizeof(Persona *));
        vet[i]->nome = malloc(strlen(nome) + 1);
        vet[i]->cognome = malloc(strlen(cognome) + 1);
        strcpy(vet[i]->nome, nome);
        strcpy(vet[i]->cognome, cognome);
        vet[i]->sesso = sesso;
        vet[i]->annoNascita = annoNascita;

        i++;
    }

    printVet(vet);
    Persona min, max;
    find_max_min(vet, &min, &max);

    printf("\nMax :\t");
    printPersona(max);

    printf("Min :\t");
    printPersona(min);

    // deallocazione memoria
    for (int i = 0; i < i; i++)
    {
        free(vet[i]->nome);
        free(vet[i]->cognome);
        free(vet[i]);
    }

    rewind(fin); // riporta il puntatore del file all'inizio

    // cancella Vito dal file di testo
    i = 0;
    while (fscanf(fin, "%s %s %c %i", nome, cognome, &sesso, &annoNascita) != EOF && i < DIM)
    {
        if (strcmp(nome, "Vito") != 0)
            fprintf(fout, "%s %s %c %i\n", nome, cognome, sesso, annoNascita);

        i++;
    }

    fclose(fin);
    fclose(fout);

    remove("input.txt");               // elimina il file originale
    rename("output.txt", "input.txt"); // rinomina il file temporaneo

    puts("\nFile dopo l'eliminazione dei record:\n");
    system("cat input.txt");

    return 0;
}