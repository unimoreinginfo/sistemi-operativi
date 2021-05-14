/* Author: Lorenzo Rutayisire */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

typedef int pipe_t [2];
typedef char line_t [255];

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    return casuale;
}


int main(int argc, char* argv[])
{
    argc--;

    if (argc < 5) {
        printf("Sintassi errata: %s <F0> <F1> <F2> <F3> ... <H>\n", argv[0]);
        exit(1);
    }

    argv++;

    srand(time(NULL));

    // H e' il numero di linee di ogni file, garantito essere uguale per tutti.
    int H = atoi(argv[argc - 1]);
    if (H == 0 || H >= 255) {
        printf("H dev'essere un numero intero strettamente positivo < 255\n");
        exit(2);
    }

    int Fcreato = open("/tmp/creato", O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0644);
    if (Fcreato < 0) {
        printf("Creazione file temporaneo fallita\n");
        exit(3);
    } 
 
    int N = argc - 1;

    pipe_t* child_to_parent_pipe = (pipe_t*) malloc(N * sizeof(pipe_t)); // child to parent
    pipe_t* parent_to_child_pipe = (pipe_t*) malloc(N * sizeof(pipe_t)); // parent to child 

    if (child_to_parent_pipe == NULL || parent_to_child_pipe == NULL) {
        printf("malloc() fallita\n");
        exit(3);
    }

    for (int i = 0; i < N; i++)
    {
        if (pipe(child_to_parent_pipe[i]) < 0 || pipe(parent_to_child_pipe[i]) < 0) {
            printf("Creazione pipe fallita\n");
            exit(4);
        }
    }

    for (int i = 0; i < N; i++)
    {
        printf("[padre] creazione figlio %d per file: %s\n", i, argv[i]);

        int pid;
        if ((pid = fork()) < 0) {
            printf("Fork fallita\n");
            exit(5);
        }

        if (pid == 0)
        { /* figlio */
            int fd = open(argv[i], O_RDONLY, 0644);
            if (fd < 0) {
                printf("Errore nella creazione del file\n");
                exit(-1);
            }

            line_t* lines = (line_t*) malloc(H * sizeof(line_t));
            int* lines_char_cnt = (int*) malloc(H * sizeof(int));

            if (lines == NULL || lines_char_cnt == NULL) {
                printf("malloc() fallita\n");
                exit(-1);
            }

            char c;
            int line_idx = 0;
            int char_cnt = 0;

            while (read(fd, &c, sizeof(char)) > 0) { // legge le linee e le salva in un array
                if (c == '\n') {
                    int endl = (int) '\n';

                    lines_char_cnt[line_idx] = char_cnt;

                    //printf("[figlio] f: %s - linea: %d - letti %d caratteri\n", argv[i], lines_cnt, char_cnt);

                    write(child_to_parent_pipe[i][1], &char_cnt, sizeof(int));
                    write(child_to_parent_pipe[i][1], &endl, sizeof(int));

                    char_cnt = 0;
                    line_idx++;
                } else {
                    lines[line_idx][char_cnt] = c;
                    char_cnt++;
                }
            }

            int char_idx;
            read(parent_to_child_pipe[i][0], &char_idx, sizeof(int)); // legge l'indice del carattere

            int tot_char_written = 0;

            for (int line_idx = 0; line_idx < H; line_idx++)
            {
                if (char_idx < lines_char_cnt[line_idx])
                {
                    write(Fcreato, &lines[line_idx][char_idx], sizeof(char)); // scrittura su file tmp
                    tot_char_written++;
                }
                else
                { // indice non ammissibile per questa linea
                }
            }

            //printf("[figlio] tot_char_written=%d\n", tot_char_written);

            exit(tot_char_written);
        }
        else
        { /* padre */
        }
    }


    for (int i = 0; i < N; i++)
    {
        int num_char[255];
        int num_char_len = 0;

        int ret;
        for (int j = 0; j < H; j++) // la lunghezza in linee dei file e' sempre la stessa!
        {
            ret = read(child_to_parent_pipe[i][0], &num_char[num_char_len], sizeof(int));

            int endl;
            ret = read(child_to_parent_pipe[i][0], &endl, sizeof(int)); // ignoro il terminatore che tanto non serve

            num_char_len++;
        }

        // prendo linea random e idx di carattere random sulla linea
        int line_idx = mia_random(num_char_len);
        int line_char_idx = mia_random(num_char[line_idx]);

        printf("[padre] (figlio %d) indice carattere: %d\n", i, line_idx, line_char_idx);

        write(parent_to_child_pipe[i][1], &line_char_idx, sizeof(int));
    }

    for (int i = 0; i < N; i++)
    {
        int status;
        int pid;
        if ((pid = wait(&status)) < 0) {
            printf("Wait fallita\n");
            exit(6);
        }

        printf("[padre] (figlio) pid: %d - caratteri scritti: %d\n", pid, ((status >> 8) & 0xff));
    }

    return 0;
}
