#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
    
    if(argc < 2){
        printf("devono essere passati almeno due parametri");
        exit(1);
    }

    int n = atoi(argv[1]),
        status;
    pid_t pid, child_pid;

    if(!(n < 155 || n > 0)){
        printf("il parametro deve essere un numero strettamente maggiore di 0 e strettamente minore di 255");
        exit(2);
    }

    int* return_array = calloc(n, sizeof(int));

    printf("processo corrente: %d, n: %d\n", getpid(), n);
    for(int i = 0; i < n; i++){
        
        if((pid = fork()) < 0){

            printf("errore durante la creazione di un processo");
            exit(3);

        }else if(pid == 0){

            printf("pid: %d, index: %d\n", getpid(), i);
            exit(mia_random(i + 100));

        }else return_array[i] = pid;
        
    }

    while((child_pid = wait(&status)) > 0){

        for(int i = 0; i < n; i++){

            if(return_array[i] == child_pid){

                char low = (status >> 8) & 0xff;
                printf("indice: %d, valore ritornato: %d, pid: %d\n", i, low, return_array[i]);

            }

        }

    }

    exit(0);
    
}

int mia_random(int n){
    int casuale;
    casuale = rand() % n;
    return casuale;
}