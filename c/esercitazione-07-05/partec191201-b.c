#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>


struct CharOcc
{
    char C;
    int occ;
};

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

    int piped[2];
    if (pipe(piped) < 0)
    {
        printf("Creazione pipe fallita\n");
        exit(2);
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
            close(piped[0]); // chiude pipe in lettura

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
            
            struct CharOcc char_occ = {
                .C = C[i][0],
                .occ = occ
            };
            write(piped[1], &char_occ, sizeof(struct CharOcc));

            close(f);

            exit(0);
        }
        else
        { /* padre */
        }
    }

    close(piped[1]); // chiude pipe in scrittura

    int ret;
    struct CharOcc char_occ;
    while ((ret = read(piped[0], &char_occ, sizeof(struct CharOcc))) > 0)
    {
        printf("%c: %d\n", char_occ.C, char_occ.occ);        
    }

    for (int i = 0; i < N; i++)
    {

        int status;
        int pid = wait(&status);

        printf("pid=%d status=%d\n", pid, status);
    }

}
