#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

typedef int piped_t[2];
typedef struct {

    int c1; // pid nipote
    int c2; // lunghezza linea
    char c3[250]; // linea

} item;

int main(int argc, char** argv){

    int N = argc - 1;
    piped_t* pipes;

    if(N < 3){
        
        printf("almeno 3 parametri");
        exit(1);

    }

    pipes = malloc(sizeof(piped_t) * N);
    if(!pipes){
        printf("errore nell'allocazione di memoria per le pipe");
        exit(2);
    }
    for(int i = 0; i < N; i++){

        if(pipe(pipes[i]) != 0){
            printf("errore nella generazione delle pipe");
            exit(2);
        }

    }

    for(int i = 0; i < N; i++){

        int P;

        if((P = fork()) < 0){
            printf("errore nella generazione dei figli");
            exit(2);
        }

        if(P == 0){

            int h = i + 1;
            char* F = argv[h];

            for(int j = 0; j < N; j++){

                close(pipes[j][0]);
                if(j != i) { 
                    close(pipes[j][1]); 
                } 

            }

            int n_c[2]; // pipe figlio-nipote

            if(pipe(n_c) < 0){
                printf("errore nella creazione della pipe figlio-nipote\n");
                exit(-1);
            }

            int nep;
            if((nep = fork()) < 0){
                printf("errore nella generazione di un nipote\n");
                exit(-1);
            }

            if(nep == 0){
                
                close(n_c[0]); // chiudo lettura sul nipote

                close(1); // chiudo lo stdout
                dup(n_c[1]); // lo ridirigo sulla pipe
                close(n_c[1]);

                close(0); // chiudo lo stdin
                int fd = open(F, O_RDONLY);

                if(fd < 0) exit(-1);

                execlp("sort", "sort", (char*) 0); // riordino il file, a questo punto l'output viene mandato sulla pipe

                exit(-1);

            }

            close(n_c[1]); // chiudo scrittura sul figlio

            int c = 0;
            item sendable; // = { .c1 = nep, .c2 = 0, .c3 = &line };

            while(read(n_c[0], &(sendable.c3[c]), sizeof(char)) > 0){
                    
                if(sendable.c3[c] == '\n'){

                    sendable.c3[++c] = 0; // termino la linea
                    break; // ho ricevuto la prima linea

                } else c++;

            }

            
            sendable.c1 = nep;
            sendable.c2 = c;

            write(pipes[i][1], &sendable, sizeof(sendable));

            exit(sendable.c2 - 1);

        }

    }

    for(int i = 0; i < N; i++){
        
        item* sent = malloc(sizeof(item));
        read(pipes[i][0], sent, sizeof(item));

        int size = sent->c2;
        char* str = malloc(sizeof(char) * size);

        memcpy(str, sent->c3, size);

        printf("file: %s\n", argv[i + 1]);
        printf("c1: %d\n", sent->c1);
        printf("c2: %d\n", size);
        printf("linea: %s\n", sent->c3);

    }

    for(int i = 0; i < N; i++){

        int pid, status;

        if((pid = wait(&status)) < 0){
            printf("errore in wait!");
            exit(2);
        }

        status = WEXITSTATUS(status);
        printf("pid %d ha ritornato %d\n", pid, status);

    }

}