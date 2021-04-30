

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Sintassi errata! Servono 2 parametri.\n");
        exit(1);
    }

    // controllo file
    FILE* F = fopen(argv[1], "r");
    if (!F)
    {
        printf("Il file non esiste: %s\n", argv[1]);
        exit(2);
    }

    // controllo carattere
    if (strlen(argv[2]) != 1)
    {
        printf("Il terzo parametro deve essere un carattere: %s\n", argv[2]);
        exit(3);
    }
    char Cx = argv[2][0];

    printf("Nome eseguibile: %s\n", argv[0]);
    printf("Nome file: %s\n", argv[1]);
    printf("Cx: %c\n", Cx);

    unsigned long int cnt = 0;

    char c;
    while (!feof(F))
    {
        fread(&c, 1, sizeof(char), F);

        if (c == Cx) {
            cnt++;
        }
    }

    printf("Numero di occorrenze: %d\n", cnt);

    fclose(F);

    return 0;
}