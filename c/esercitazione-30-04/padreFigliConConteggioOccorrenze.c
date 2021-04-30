#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){
    
    if(argc < 3){
        printf("devono essere passati almeno tre parametri");
        exit(1);
    }
    
    char* last_arg = argv[argc - 1];

    if(last_arg[1] != 0){
        printf("l'ultimo elemento deve essere un carattere");
        exit(1);
    }
    
    for(int i = 0; i < argc - 2; i++){
        
        if(access(argv[i + 1], F_OK) != 0){
            printf("tutti gli n argomenti devono essere file esistenti!");
            exit(2);
        }
        
    }

    pid_t pid, child_pid;
    int status;

    for(int i = 0; i < argc - 2; i++){        
        
        if((pid = fork()) < 0){

            printf("errore durante la creazione di un processo");
            exit(3);

        }else if(pid == 0){

            int pointer = open(argv[i + 1], O_RDONLY),
                n = 0;
            
            while (1) {
                
                char buffer;
                ssize_t r_status = read(pointer, &buffer, 1);

                if (r_status <= 0) break;
                if(buffer == last_arg[0]) n++;

            }

            close(pointer);

            exit(n);

        }
        
    }

    while((child_pid = wait(&status)) > 0){

        char low = (status >> 8) & 0xff;
        printf("occorrenze pid %d: %d\n", child_pid, low);

    }

    /*int n = atoi(argv[1]);
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

    }*/

    exit(0);
    
}