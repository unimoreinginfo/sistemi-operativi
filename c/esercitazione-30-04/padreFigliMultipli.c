#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
    
    if(argc != 2){
        printf("deve essere passato un solo parametro");
        exit(1);
    }

    int n = atoi(argv[1]);
    if(!(n < 255 || n > 0)){
        printf("il parametro deve essere un numero strettamente maggiore di 0 e strettamente minore di 255");
        exit(2);
    }

    pid_t pid, child_pid;
    int status;

    printf("processo corrente: %d, n: %d\n", getpid(), n);
    for(int i = 0; i < n; i++){        
        
        if((pid = fork()) < 0){

            printf("errore durante la creazione di un processo");
            exit(3);

        }else if(pid == 0){

            printf("pid: %d, index: %d\n", getpid(), i);
            exit(i);

        }
        
    }

    while((child_pid = wait(&status)) > 0){

        char low = (status >> 8) & 0xff;
        printf("indice: %d, pid: %d\n", low, child_pid);

    }

    /*if((child_pid = wait(&status)) < 0){
            printf("errore in wait");
            exit(2);
        }
        
        if(child_pid == pid){

            char low = (status >> 8) & 0xff;
            printf("indice: %d, pid: %d\n", low, child_pid);
            
        }*/

    exit(0);
    
}