#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

int main (int argc, char **argv)
{
    if (argc <= (0 + 1))
    {
        printf("Sintassi sbagliata\n");
        exit(1);
    }

    argc--;
    argv++;

    int digit_piped[2];
    int alpha_piped[2];

    if (pipe(digit_piped) < 0 || pipe(alpha_piped) < 0) {
        printf("Creazione della pipe fallita\n");
        exit(1);
    }

    for (int i = 0; i < argc; i++)
    {
        int pid;
        if ((pid = fork()) < 0)
        {
            printf("Fork fallita\n");
            exit(2);
        }

        if (pid == 0)
        { /* figlio */

            close(alpha_piped[0]); // chiude la pipe in lettura
            close(digit_piped[0]); // chiude la pipe in lettura

            int f = open(argv[i], O_RDONLY, 0644);
            if (f < 0) {
                printf("Errore durante l'apertura del file: %s\n", argv[i]);
                exit(3);
            }

            if (i % 2 == 0) { close(alpha_piped[1]); }
            if (i % 2 != 0) { close(digit_piped[1]); }

            char c;
            while (read(f, &c, sizeof(char)) > 0)
            {
                if (i % 2 == 0 && isdigit(c)) write(digit_piped[1], &c, sizeof(char));
                if (i % 2 != 0 && isalpha(c)) write(alpha_piped[1], &c, sizeof(char));
            }

            close(f);

            exit(0);
        }
        else
        { /* padre */
        }
    }

    close(alpha_piped[1]); // chiude la pipe in scrittura
    close(digit_piped[1]); // chiude la pipe in scrittura

    char alpha, digit;
    int ret_alpha, ret_digit;

    while ((ret_alpha = read(alpha_piped[0], &alpha, sizeof(char))) > 0 || (ret_digit = read(digit_piped[0], &digit, sizeof(char))) > 0)
    {
        if (ret_alpha > 0) printf("carattere: %c\n", alpha);
        if (ret_digit > 0) printf("digit: %c\n", digit);
    }
}
