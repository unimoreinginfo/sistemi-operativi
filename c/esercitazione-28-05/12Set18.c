#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>

typedef int piped_t[2];

int main(int argc, char** argv){

    int N = argc - 1;

    if(N < 2){

        printf("servono almeno due parametri\n");
        exit(1);

    }
    
    int* children = malloc(sizeof(int) * N);
    piped_t* pipes = malloc(sizeof(piped_t) * N);
    piped_t* nep_pipes = malloc(sizeof(piped_t) * N);

    for(int i = 0; i < N; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore durante la creazione di una pipe");
            exit(2);

        }

        if(pipe(nep_pipes[i]) != 0){

            printf("errore durante la creazione di una pipe del nipote");
            exit(2);

        }

    }

    for(int i = 0; i < N; i++){

        int P = 0;
        if((P = fork()) < 0){
            printf("errore durante la creazione di un figlio");
            exit(2);
        }

        if(P == 0){
            
            int nep;
            int child_file = open(argv[i + 1], O_RDWR);

            for(int q = 0; q < N; q++){

                close(pipes[q][0]);
                if(q != i){
                    close(pipes[q][1]);
                }

            }

            // printf("%d spawned\n", getpid());

            if((nep = fork()) < 0){
                printf("errore in fork nipote");
                exit(-1);
            }

            if(nep == 0){
                
                // printf("(n) %d spawned\n", getpid());
                int nep_file = open(argv[i + 1], O_RDWR);
                long int count = 0;
                char buf;
                // nipote deve cercare i caratteri alfabetici minuscoli
                while(read(nep_file, &buf, sizeof(char)) > 0){
                    
                    // printf("%c\n", buf);

                    for(int q = 0; q < N; q++){

                        close(nep_pipes[q][0]);
                        if(q != i){
                            close(nep_pipes[q][1]);
                        }

                    }

                    if(buf >= 'a' && buf <= 'z') {
                        lseek(nep_file, -1L, SEEK_CUR); // mi sposto indietro di uno per scrivere sul file
                        count++;
                        char upper = buf - 32;
                        write(nep_file, &upper, sizeof(char));
                    }

                }

                // printf("(n) %d sta terminando\n", getpid());

                write(nep_pipes[i][1], &count, sizeof(long int));
                exit(count / 256);

            }
                
            long int child_count = 0;
            char child_buf;

            while(read(child_file, &child_buf, sizeof(char)) > 0){

                if(isdigit(child_buf)){
                    lseek(child_file, -1L, SEEK_CUR);
                    child_count++;
                    char space = ' ';
                    write(child_file, &space, sizeof(char));
                }

            }

            int n_status, n_pid;

            if((n_pid = wait(&n_status)) < 0){
                printf("errore durante la wait di un nipote\n");
                exit(-1);
            }

            printf("nipote %d ha ritornato %d\n", n_pid, WEXITSTATUS(n_status));

            write(pipes[i][1], &child_count, sizeof(long int));

            exit(child_count / 256);
            

        }else children[i] = P;

    }

    for(int i = 0; i < N; i++){

        close(pipes[i][1]);
        close(nep_pipes[i][1]);

    }

    for(int i = 0; i < N; i++){

        long int child_read, nep_read;
        read(pipes[i][0], &child_read, sizeof(long int));
        read(nep_pipes[i][0], &nep_read, sizeof(long int));

        printf("%s ha subito %ld trasformazioni da parte dal figlio\n", argv[i + 1], child_read);
        printf("%s ha subito %ld trasformazioni da parte dal nipote\n", argv[i + 1], nep_read);

    }

    for(int i = 0; i < N; i++){

        int child_pid, status;

        if((child_pid = wait(&status)) < 0){
            printf("errore in wait");
            exit(2);
        }

        status = WEXITSTATUS(status);
        printf("figlio di pid %d ha ritornato %d\n", child_pid, status);

    }

}