#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];
typedef struct {
    long int c1;
    long int c2;
} item;

int main(int argc, char** argv){

    int N = argc - 1;
    if(N < 2 || N % 2 != 0){

        printf("N dev'essere maggiore/uguale a 2 e pari!");
        exit(1);

    }

    piped_t* pipes = (piped_t*) malloc(sizeof(piped_t) * N);
    
    if(!pipes){
        printf("errore nell'allocazione delle pipe");
        exit(2);
    }

    for(int i = 0; i < N; i++){

        if(pipe(pipes[i]) < 0){
            printf("errore nella generazione delle pipe");
            exit(2);
        }

    }

    for(int i = 0; i < N; i++){

        int P = fork();

        if(P < 0) exit(2);
        if(P == 0){
            
            for(int j = 0; j < N; j++){

                close(pipes[j][0]);
                if(j != i) close(pipes[j][1]);

            }

            char* filename = argv[i + 1];
            int is_even = !(i % 2);
            
            long int c1 = 0, c2 = 0;
            int fd = open(filename, O_RDONLY);

            if(fd < 0) exit(-1);

            char buf;
            long int position;

            while(read(fd, &buf, sizeof(char))){

                if(is_even){
                    
                    if(position % 2 == 0){

                        if(buf % 2 == 0) c1++;
                        else c2++;

                    }

                }else{

                    if(position % 2 != 0){

                        if(buf % 2 == 0) c2++;
                        else c1++;

                    }

                }

                position++;

            }

            item sendable = { .c1 = c1, .c2 = c2 };
            write(pipes[i][1], &sendable, sizeof(sendable));

            (is_even) ? exit(c1 < c2) : exit(c2 > c1);

        }

        close(pipes[i][1]);

    }

    printf("stampa figli pari\n");
    for(int i = 0; i < N; i += 2){

        // solo figli pari
        item* sent = malloc(sizeof(item));
        read(pipes[i][0], sent, sizeof(item));

        printf("figlio di indice %d ha riportato c1 = %ld e c2 = %ld\n", i, sent->c1, sent->c2);

    }
    printf("\nstampa figli dispari\n");
    for(int i = 1; i < N; i += 2){

        // solo figli dispari
        item* sent = malloc(sizeof(item));
        read(pipes[i][0], sent, sizeof(item));

        printf("figlio di indice %d ha riportato c1 = %ld e c2 = %ld\n", i, sent->c1, sent->c2);

    }
    printf("\n");
    for(int i = 0; i < N; i++){

        int pid, status;

        if((pid = wait(&status)) < 0){

            printf("errore in wait");
            exit(2);

        }

        printf("%d ritorna %d\n", pid, WEXITSTATUS(status));

    }

}