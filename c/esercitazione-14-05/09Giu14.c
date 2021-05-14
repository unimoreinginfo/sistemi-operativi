/* Author: macca */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>

typedef int pipe_t[2];

int main(int argc, char** argv){

    if(argc < 3){

        printf("numero parametri insufficiente\n");
        exit(1);

    }

    int status, child_pid;
    long int sum = 0;
    pipe_t *big_pipe = malloc(sizeof(pipe_t) * (argc - 1));

    for(int i = 0; i < argc - 1; i++){

        if(pipe(big_pipe[i]) < 0){
            printf("errore nella creazione di big_pipe\n");
            exit(2);
        }
    }
    
    for(int k = 0; k < argc - 1; k++){

        int pid;
        if((pid = fork()) < 0){

            printf("fork fallita\n");
            exit(2);

        }
        if(pid == 0){

            // figlio
            char *filename = argv[k + 1];
            int piped[2];

            for(int j = 0; j < argc - 1; j++){
                
                close(big_pipe[j][0]);

                if(j != k)
                    close(big_pipe[j][1]);

            }

            printf("filename: %s\n", filename);

            if(pipe(piped) < 0){
                printf("errore creazione pipe\n");
                exit(1);
            }

            int nep_pid, status;
            
            if((nep_pid = fork()) < 0){

                printf("fork del nipote fallita\n");
                exit(3);

            }
            if(nep_pid == 0){
                
                close(piped[0]); // lato lettura
                
                close(1);
                dup(piped[1]);

                close(0);
                int fd = open(filename, O_RDONLY);
                
                execlp("wc", "wc", "-l", (char*) 0);

                exit(-1);

            }else{
                
                char lines[128];
                int i = 0, child_status;
                close(piped[1]); // lato scrittura chiuso su figlio

                if((nep_pid = wait(&child_status)) < 0){
                    printf("errore in wait");
                    exit(4);
                }
                
                while(1){

                    if(read(piped[0], &lines[i], 1) <= 0) 
                        break;
                    
                    if(lines[i] == '\n'){

                        lines[i] = '\0';
                        break;

                    }

                    i++;
                    
                }

                long int res = atoi(lines);
                write(big_pipe[k][1], &res, sizeof(long int));

                exit(child_status);

            }

        }

    }

    long int final_sum;

    for(int i = 0; i < argc - 1; i++){

        long int item;
        read(big_pipe[i][0], &item, sizeof(long int));    

        final_sum += item;

    }

    printf("somma finale: %ld\n", final_sum);

    for(int i = 1; i < argc; i++){

        if((child_pid = wait(&status)) < 0){

            printf("errore nella wait del padre\n");
            exit(5);

        }

        printf("pid %d ha ritornato %d\n", child_pid, status);

    }

    exit(0);

}