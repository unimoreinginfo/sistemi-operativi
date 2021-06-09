#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];

int main(int argc, char** argv){

    int Q = argc - 1;

    if(Q < 2){
        
        printf("devono esserci almeno 2 parametri");
        exit(1);

    }

    piped_t* pipes = (piped_t*) malloc(sizeof(piped_t) * Q);
    char s = 'x';

    if(!pipes){
        printf("errore in malloc");
        exit(3);
    }

    for(int p = 0; p < Q; p++){

        if(pipe(pipes[p]) < 0){
            printf("errore nell'allocazione delle pipe");
            exit(4);
        }

    }

    write(pipes[0][1], &s, sizeof(char));

    for(int q = 0; q < Q; q++){

        int P;

        if((P = fork()) < 0){
            printf("errore in fork");
            exit(2);
        }

        if(P == 0){

            // todo: chiusura pipe

            int friend = (q + 1) % Q;
            int read_channel = pipes[q][0],
                write_channel = pipes[friend][1];

            char* F = argv[q + 1],
            line[255];
            char buf, sync_token;
            int file = open(F, O_RDONLY), current_index = 0, numeric_chars = 0, line_index = 1, last_line;

            while(read(file, &buf, sizeof(char)) > 0){

                line[current_index] = buf;

                if(buf >= '0' && buf <= '9')
                    numeric_chars++;

                if(buf == '\n'){

                    // line break;
                    line[current_index] = 0; // terminiamo la stringa creata finora (elido \n)
                    current_index = 0; // resetto la posizione

                    if(read(read_channel, &sync_token, sizeof(char)) < 0){ // blocco qui finché non ricevo l'OK dal figlio precedente
                        printf("errore nella sincronizzazione (grave)!");
                        exit(-1);
                    }

                    printf("pid: %d (indice %d), linea (%d): %s, numero caratteri numerici: %d\n", getpid(), q, line_index++, line, numeric_chars);

                    write(write_channel, &s, sizeof(char));

                    line[0] = 0; // resetto completamente la stringa
                    last_line = numeric_chars;
                    numeric_chars = 0; // resetto i caratteri numerici trovati

                }else current_index++;

            }

            exit(last_line);

        }

    }

    for(int q = 0; q < Q; q++){

        int child_pid, status;

        if((child_pid = wait(&status)) < 0){

            printf("errore nell'attesa");
            exit(3);

        }

        printf("%d ha ritornato %d\n", child_pid, WEXITSTATUS(status));

    }

}