#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>


int main (int argc, char **argv)
{
    if (argc <= (2 + 1))
    {
        printf("Sintassi sbagliata: <F> <C1> ... <CN>\n");
        exit(1);
    }

    argc--;
    argv++;

    char** C = argv + 1;
    int N = argc - 1;

    int* piped = (int*) malloc(2 * N * sizeof(int));
    if (piped < 0)
    {
        printf("malloc() fallita\n");
        exit(2);
    }

    for (int i = 0; i < N; i++)
    {
        if (pipe(&piped[2 * i]) < 0)
        {
            printf("Pipe fallita\n");
            exit(3);
        }
    }
    

    for (int i = 0; i < N; i++)
    {
        int pid;
        if ((pid = fork()) < 0)
        {
            printf("Fork fallita\n");
            exit(4);
        }

        if (pid == 0)
        { /* figlio */
            int f = open(argv[0], O_RDONLY, 0644);
            if (f < 0) {
                printf("Errore nell'apertura di: %s\n", argv[0]);
                exit(5);
            }

            int occ = 0;
            char c;
            while (read(f, &c, sizeof(char)) > 0) {
                if (c == C[i][0]) {
                    occ++;
                }
            }
            
            write(piped[2 * i + 1], &occ, sizeof(int));

            close(f);

            exit(0);
        }
        else
        { /* padre */
        }
    }
    
 

    for (int i = 0; i < N; i++)
    {
        int ret, occ;
        ret = read(piped[2 * i], &occ, sizeof(int));
        printf("%c: %d\n", C[i][0], occ);
    }

    for (int i = 0; i < N; i++)
    {
        int status;
        int pid = wait(&status);

        printf("pid=%d status=%d\n", pid, status);
    }

}
