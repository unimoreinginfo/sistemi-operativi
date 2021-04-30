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
    
    for(int i = 1; i < argc; i++){
        
        if(access(argv[i], F_OK) != 0){
            printf("tutti gli n argomenti devono essere file esistenti!");
            exit(2);
        }
        
    }

    pid_t pid, child_pid;
    int status;

    for(int i = 1; i < argc; i++){        
        
        int n_status;
        if((pid = fork()) < 0){

            printf("errore durante la creazione di un processo");
            exit(3);

        }else if(pid == 0){
            
            char* formatted;
            sprintf(formatted, "./%s.sort", argv[i]);
            printf("%s\n", formatted);
            pid_t n_pid;
            
            if((n_pid = fork()) < 0){
                printf("errore durante la creazione di un processo nipote");
                exit(4);
            }else if(n_pid == 0){
                
                // nipote
                close(1);
                open(formatted, O_WRONLY | O_CREAT, 0644); 
                close(0);
                open(argv[i], O_RDONLY);
                execlp("sort", "sort", (char*) 0);

                exit(-1);

            }else{

                // processo zio
                if((n_pid = wait(&status)) < 0){
                    printf("errore in wait");   
                }

                exit(status);

            }

        }
        
    }

    while((child_pid = wait(&status)) > 0){

        char low = (status >> 8) & 0xff;
        printf("sort status pid %d: %d\n", child_pid, low);

    }

    exit(0);
    
}