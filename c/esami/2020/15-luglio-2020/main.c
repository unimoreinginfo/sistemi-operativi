#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];
typedef struct {
    pid_t c1;
    char c2;
    char c3;
} item;

void close_pipes(piped_t*, int, int);

int main(int argc, char** argv){

    char* F = argv[1]; // nome file
    int L   = atoi(argv[2]); // linee file

    if(argc > 3){
        printf("esattamente 2 parametri");
        exit(1);
    }

    if(L <= 0 || L > 255){
        printf("L deve essere strettamente positivo e <= 255");
        exit(1);
    }

    piped_t* pipes = malloc(sizeof(piped_t) * L);
    if(!pipes){
        printf("errore durante l'allocazione della memoria per le pipe");
        exit(2);
    }

    for(int i = 0; i < L; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore in pipe");
            exit(2);

        }

    }

    for(int q = 0; q < L; q++){

        int P;

        if((P = fork()) < 0){

            // printf("errore in fork");
            exit(2);

        }

        if(P == 0){
            
            // numerazione delle linee a partire da 1

            close_pipes(pipes, q, L);

            char line[250], buf;
            int file = open(F, O_RDONLY), index = 0, current_line = 1, right_line = 1 + q;
            if(file < 0){

                printf("errore in apertura file\n8");
                exit(-1);

            }

            while(read(file, &buf, sizeof(char)) > 0){

                if(buf == '\n'){
                    
                    line[index] = 0;
                    
                    if(current_line == right_line){

                        // printf("(indice figlio: %d) letta la linea (%d): %s\n", q, current_line, line);
                        char second_char = line[1],
                             last_char = line[index - 1];
                        
                        item sendable = {
                            .c1 = getpid(),
                            .c2 = second_char,
                            .c3 = last_char
                        };
                        
                        // printf("%c - %c\n", first_char, last_char);
                        write(pipes[q][1], &sendable, sizeof(item));

                        break;

                    }

                    index = 0;

                    current_line++;

                } else line[index++] = buf;

            }

            exit(right_line);

        } else close(pipes[q][1]); // chiudo la scrittura sul padre 

    }

    for(int q = 0; q < L; q++){

        item* read_item = malloc(sizeof(item));
        if(!read_item) perror("addio");
        if(read(pipes[q][0], read_item, sizeof(item)) < 0) perror("errore in read nel padre");

        if(read_item->c2 == read_item->c3){

            printf("file associato: %s, numero linea: %d\n", F, q + 1);
            printf("c1: %d\n", (int) read_item->c1);
            printf("c2: %c\n", (char) read_item->c2);
            printf("c3: %c\n\n", (char) read_item->c3);

        }

    }

    for(int i = 0; i < L; i++){

        int pid, status;
        if((pid == wait(&status)) < 0){
            printf("oh no (wait)");
            exit(2);
        }

        printf("figlio %d ha ritornato %d\n", pid, WEXITSTATUS(status));

    }

}

void close_pipes(piped_t* pipes, int current_index, int pipe_len){

    for(int i = 0; i < pipe_len; i++){

        close(pipes[i][0]); // chiudo la lettura

        if(i != current_index){

            close(pipes[i][1]); // chiudo la scrittura perché non serve

        }

    }

}