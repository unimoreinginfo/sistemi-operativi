
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        printf("Sintassi invalida: %s <p1> ...\n", argv[0]);
        exit(1);
    }

    printf(argv[0]);
    printf("\n");

    printf("Numero di parametri: %d\n", (argc - 1));

    for (int i = 1; i < argc; i++)
    {
        printf("%d) %s\n", i, argv[i]);
    }
}
