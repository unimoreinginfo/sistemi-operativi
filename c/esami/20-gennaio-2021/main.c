#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef int piped_t[2];

int length(int);

int main(int argc, char** argv){

    int Q = argc - 1;
    char start = 'x';
    
    if(Q < 2){
        printf("minimo due parametri");
        exit(1);
    }

    piped_t* pipes = malloc(sizeof(piped_t) * Q);

    if(!pipes){

        printf("errore durante l'allocazione della memoria per le pipe");
        exit(2);

    }

    for(int i = 0; i < Q; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore durante la creazione delle pipe");
            exit(2);

        }

    }

    write(pipes[0][1], &start, sizeof(char)); // padre fa partire il ciclo di sincronizzazione

    for(int q = 0; q < Q; q++){

        int P;
        if((P = fork()) < 0){
            
            printf("errore in fork");
            exit(2);

        }

        if(P == 0){

            char* filename = argv[q + 1];
            int file = open(filename, O_RDONLY);

            if(file < 0){

                printf("errore durante l'apertura del file %s\n", filename);
                exit(-1);

            }

            int read_channel = pipes[q][0],
                write_channel = pipes[(q + 1) % Q][1],
                k = 0, file_length = length(file);
            
            for(int i = 0; i < Q; i++){

                if(i != (q + 1) % Q) close(pipes[i][1]);
                if(i != q) close(pipes[i][0]);

            }

            char sync_char, read_char;

            while(read(read_channel, &sync_char, sizeof(char)) > 0){ // aspetta la sync dal figlio precedente (o dal padre))

                int position = q + k * Q, current_position = 0;

                while(read(file, &read_char, sizeof(char)) > 0){

                    if(current_position == position){
                        
                        // printf("figlio %d scrive sul canale pipes[%d][1]", q, (q + 1) % Q);
                        printf("figlio con indice %d ha letto %c (posizione: %d)\n", q, read_char, position);
                        write(write_channel, &start, sizeof(char));
                        lseek(file, 0L, SEEK_SET);

                        /*
                            ho girato in modo strano la consegna: 
                            sostanzialmente il figlio rilegge il file ogni volta (non avevo pensato di farlo in un altro modo)
                            e invia ogni volta che arriva alla posizione desiderata finché non arriva alla fine del file (o dopo)
                            questo l'ho fatto poiché la sincronizzazione avviene all'inizio e non durante la la scrittura sul canale
                            direi vada bene lo stesso (spero)
                        */

                        break;

                    } 

                    current_position++;

                }

                if(current_position >= file_length) { write(write_channel, &start, sizeof(char)); break; };
                
                k++;

            }

            exit(q); 
            
        }

    }

    for(int q = 0; q < Q; q++){

        int status, pid;
        if((pid = wait(&status)) < 0){

            printf("errore in wait");
            exit(2);

        }

        status = WEXITSTATUS(status);
        printf("%d ritorna %d\n", pid, status);

    }

}

int length(int fd){

    int len = 0;
    char c;
    lseek(fd, 0L, SEEK_SET);

    while(read(fd, &c, sizeof(char)) > 0){

        len++;

    }

    lseek(fd, 0L, SEEK_SET);
    return len;

}