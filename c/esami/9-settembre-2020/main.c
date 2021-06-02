#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef int piped_t[2];

bool readline = false;

void h1(int sig){
    readline = true;
}
void h2(int sig){
    readline = false;
}

int main(int argc, char** argv){

    signal(SIGUSR1, h1);
    signal(SIGUSR2, h2);

    int params = argc - 1;
    
    if(params < 5){

        printf("ci devono essere almeno 4 parametri");
        exit(1);

    }

    int Q = params - 2;

    char* F = argv[1];
    int L = atoi(argv[2]);
    pid_t* children = (pid_t*) malloc(sizeof(pid_t) * Q);
    pid_t parent = getpid();

    if(L <= 0){

        printf("L deve essere > 0");
        exit(1);

    }

    for(int i = 0; i < Q; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore durante la generazione delle pipes");
            exit(2);

        }

    }

    for(int q = 0; q < Q; q++){

        char associated_char = argv[3 + q][0];
        int P;
        
        if((P = fork()) < 0){

            printf("errore in fork\n");
            exit(2);

        }

        children[q] = P;

        if(P == 0){

            int file = open(F, O_RDONLY);
            if(file < 0){
                printf("errore durante l'apertura del file %s\n", F);
                exit(-1);
            }
            int nr, occ = 0, last_occ;
            char buf;

            while((nr = read(file, &buf, sizeof(char))) > 0){
                
                if(nr < sizeof(char)){
                    printf("bruh\n");
                    exit(-1);
                }

                if(buf == associated_char)
                    occ++;

                if(buf == '\n'){

                    // letta una linea intera
                    last_occ = occ;
                    pause();
                    printf("%d occorrenza/e del carattere '%c'\n", occ, associated_char);
                    kill(parent, SIGUSR2);
                    occ = 0;

                }

            }
            // printf("ho letto il file boss\n");
            exit(last_occ);
        } 

    }

    for(int l = 0; l < L; l++){

        // per ogni linea
        // leggi tutte le pipe 

        printf("linea %d\n", l + 1);

        for(int q = 0; q < Q; q++){

            kill(children[q], SIGUSR1);
            pause();

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
