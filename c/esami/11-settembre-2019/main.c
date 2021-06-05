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

    int N = argc - 1;

    if(N < 2 || N % 2 != 0){

        printf("N dev'essere pari e maggiore o uguale a due");
        exit(1);

    }

    piped_t* pipes = (piped_t*) malloc(sizeof(piped_t) * N);

    if(!pipes){

        printf("errore in malloc");
        exit(2);

    }

    for(int i = 0; i < N; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore nella creazione delle pipe");
            exit(2);

        }

    }

    // tutti i file hanno lunghezza minore di 255
    for(int i = 0; i < N; i++){

        int P;

        if((P = fork()) < 0){

            printf("errore in fork");
            exit(2);

        }

        if(P == 0){

            char* filename = argv[i + 1];
            
            int write_channel, read_channel = pipes[i][0], fd = -1, read_chars = 0;
            if(i % 2 == 0) { 
                write_channel = pipes[i + 1][1];

                for(int q = 0; q < N; q++){

                    if(q != i + 1){
                        close(pipes[q][0]);
                        close(pipes[q][1]);
                    }

                }

            }
            if(i % 2 != 0){
                
                for(int q = 0; q < N; q++){

                    if(q != i)
                        close(pipes[q][0]);

                    close(pipes[q][1]);

                }
                // figlio dispari
                char* str_file = malloc(sizeof(char) * 255);
                sprintf(str_file, "%s.MAGGIORE", filename);
                fd = open(str_file, O_CREAT | O_WRONLY, 0644);

            }

            int related_file = open(filename, O_RDONLY);

            char buf;
            while(read(related_file, &buf, sizeof(char)) > 0){
                
                read_chars++;

                if(i % 2 == 0)
                    write(write_channel, &buf, sizeof(char));
                else{

                    char other;
                    read(read_channel, &other, sizeof(char));
                    if(buf > other) write(fd, &buf, sizeof(char));
                    else write(fd, &other, sizeof(char));
                
                }

            }

            exit(read_chars);

        }

    }

    for(int i = 0; i < N; i++){
        close(pipes[i][1]);
        close(pipes[i][0]);
    }

    for(int i = 0; i < N; i++){

        int status, pid;

        if((pid = wait(&status)) < 0){
            printf("errore in wait");
            exit(2);
        }

        status = WEXITSTATUS(status);
        printf("%d ha ritornato %d\n", pid, status);

    }

}