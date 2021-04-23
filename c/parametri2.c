


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
    if (argc != (1 + 3))
    {
        printf("Sintassi errata: %s <1> <2> <3>\n", argv[0]);
        exit(1);
    }

    // controllo file
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        printf("Il file non esiste: %s\n", argv[1]);
        exit(2);
    }

    // controllo secondo parametro numero
    if (atoi(argv[2]) == 0)
    {
        printf("Il secondo parametro deve essere un numero: %s\n", argv[2]);
        exit(3);
    }

    // controllo carattere
    if (strlen(argv[3]) != 1)
    {
        printf("Il terzo parametro deve essere un carattere: %s\n", argv[3]);
        exit(4);
    }

    printf("Nome exec: %s | Numero di param: %d\n", argv[0], argc);

    printf("Primo param: %s\n", argv[1]);
    printf("Secondo param: %s\n", argv[2]);
    printf("Terzo param: %c\n", argv[3][0]);

    fclose(f);
}