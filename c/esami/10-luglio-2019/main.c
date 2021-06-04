#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

typedef int piped_t[2];

int main(int argc, char** argv){

    // vengono passati N + 1 parametri
    int N = argc - 2;
    long int start = 0;
    if(N < 2){
        printf("errore nel numero dei parametri");
        exit(1);
    }

    char c = argv[N + 1][0];
    printf("c: %c\n", c);
    printf("numero figli: %d * 2 = %d\n", N, N * 2);

    piped_t* pipes = malloc(sizeof(piped_t) * (N * 2));

    for(int i = 0; i < N * 2; i++){

        // printf("pipe n° %d\n", i);
        if(pipe(pipes[i]) != 0){
            printf("errore in pipe");
            exit(2);
        }

    }

    for(int i = 0; i < N * 2; i++){

        int P;
        if((P = (fork())) < 0){
            exit(2);
        }

        if(P == 0){

            // printf("spawned %d\n", getpid());
            int read_channel = pipes[i][0],
                write_channel = pipes[(2 * N) - (i + 1)][1];

            if(i < N){
                write(pipes[i][1], &start, sizeof(long int));
                printf("%d viene fatto partire subito\n", i);
            }

            int file_index;
            if(i < N) file_index = i;
            else {
                file_index = 2 * N - (i + 1);
            }

            char* F = argv[file_index + 1];
            
            // printf("%d scrive su pipes[%d][1] con file %s\n", i, (2 * N) - (i + 1), F);

            int file = open(F, O_RDONLY);
            if(file < 0) exit(-1);

            for(int j = 0; j < 2 * N; j++){

                if(j != i) close(pipes[j][0]);
                if(j != (2 * N) - (i + 1)) close(pipes[j][1]);

            }

            char buf;
            long int position = 0;
            int occ = 0;
            
            // if(i < N)
            read(read_channel, &position, sizeof(long int));

            lseek(file, position, SEEK_SET);
            printf("figlio %d inizia alla posizione %ld\n", i, position);

            while(read(file, &buf, sizeof(char)) > 0){
                
                position++;
                if(buf == c){
                    
                    occ++;
                    write(write_channel, &position, sizeof(long int));
                    printf("figlio %d si blocca in posizione %ld\n", i, position);
                    read(read_channel, &position, sizeof(long int));
                    printf("figlio %d ricomincia alla posizione %ld\n", i, position + 1);
                    lseek(file, position + 1, SEEK_SET);
                
                }

            }

            // printf("%d termina\n", i);
            write(write_channel, &position, sizeof(long int));

            exit(occ);

        }else{

        }

    }

    for(int i = 0; i < 2*N; i++){

        int p, status;
        if((p = wait(&status)) < 0){
            exit(2);
        }

        status = WEXITSTATUS(status);

        printf("%d ha trovato il carattere '%c' %d volta/e\n", i, c, status);

    }

}