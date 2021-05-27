#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];
typedef struct {
    int c1;
    int c2;
} S;

int main(int argc, char** argv){

    int N = argc - 1,
        H = atoi(argv[N]);

    piped_t *fullpipe = malloc(sizeof(piped_t) * N);

    
    if(H <= 0 || H > 255){
        printf("il parametro H dev'essere compreso fra 1 e 255");
        exit(1);
    }

    for(int i = 0; i < N; i++){

        if(pipe(fullpipe[i]) < 0){
            printf("errore nella creazione della pipe");
            exit(2);
        }

    }

    // tutti gli N file hanno la stessa lunghezza H

    for(int f = 0; f < N - 1; f++){
        
        int P = fork(),
            i = 0;
        
        char* F = argv[f + 1], 
            *buffered_line, 
            **read_lines = malloc(sizeof(char*) * H);
        
        char read_char;

        for(int g = 0; g < H; g++) read_lines[i] = malloc(sizeof(char) * 255); 

        if(P < 0){

            printf("errore nella creazione di un figlio");
            exit(-1);

        }
        if(P == 0){

            // figlio
            printf("figlio %d pronto per leggere il suo file", P);
            close(2);
            int fd = open(F, O_RDONLY);
            if(fd < 0){

                printf("errore durante l'apertura del file %s", F);
                exit(-1);

            }

            int readp = fullpipe[f - 1][0],
                writep = fullpipe[f][1],
                current_line = 0;
            
            S read_struct;
            
            if(f == 0)
                close(readp);

            for(int i = 0; i < N; i++){

                if(i != f){

                    close(fullpipe[i][1]);
                    if(i != f - 1)
                        close(fullpipe[i][0]);

                }

            }

            while(read(fd, &read_char, sizeof(char)) > 0){
            
                if(read_char == '\n'){
                    
                    // invio la struttura
                    S child_struct = {
                        .c1 = f,
                        .c2 = i++
                    };

                    if(f != 0) read(readp, &read_struct, sizeof(S));

                    if(i < read_struct.c2) write(writep, &child_struct, sizeof(S));
                    else write(writep, &child_struct, sizeof(S));
                    buffered_line[i] = 0;

                    read_lines[current_line] = buffered_line;

                    current_line++;
                    i = 0;

                } else { buffered_line[i++] = read_char; }

            }

            exit(0);

        }

    }

    for(int j = 0; j < H; j++){

        S read_struct;
        read(fullpipe[N][0], &read_struct, sizeof(S));

        printf("il figlio con indice %d ha letto %d caratteri alla linea %d", read_struct.c1, read_struct.c2, j);

    }

    int status, child_pid;
    for(int k = 0; k < N; k++){

        if((child_pid = wait(&status)) < 0){

            printf("errore in wait");
            exit(4);

        }
        if((status & 0xff) != 0){
            printf("errore nel figlio %d", child_pid);
        }else{
            printf("figlio %d OK", child_pid);
        }

    }

}