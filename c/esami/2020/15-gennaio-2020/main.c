#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];

int main(int argc, char** argv){

    int N = argc - 1;
    char ok = 'k';
    if(N < 2 || N % 2 != 0){

        printf("N dev'essere maggiore/uguale a due e pari\n");
        exit(1);

    }

    int children_number = N / 2;
    for(int i = 0; i < children_number; i++){

        int P;
        if((P = fork()) < 0){
            printf("errore in fork\n");
            exit(2);
        }

        if(P == 0){

            char merge_file[255]; // abbastanza grande direi
            int child_to_nephew[2],
                nephew_to_child[2];
            
            if(pipe(child_to_nephew) != 0 || pipe(nephew_to_child) != 0) exit(-1);
            
            sprintf(merge_file, "merge%d", i);
            int merge = open(merge_file, O_WRONLY | O_CREAT, 0644);

            int nephew = fork();
            if(nephew < 0){
                printf("errore in fork\n");
                exit(-1);
            }

            if(nephew == 0){

                // nipote
                close(child_to_nephew[1]);
                close(nephew_to_child[0]);
                char* nephew_file = argv[children_number + i + 1];
                int fd_nephew = open(nephew_file, O_RDONLY);
                char nbuf;
                
                if(fd_nephew < 0) exit(-1);
                printf("il nipote %d aspetta a leggere\n", getpid());

                read(child_to_nephew[0], &ok, sizeof(char));

                while(read(fd_nephew, &nbuf, sizeof(char))){
                    
                    printf("nipote %d legge carattere %c\n", getpid(), nbuf);
                    write(merge, &nbuf, sizeof(char));
                    write(nephew_to_child[1], &ok, sizeof(char));
                    read(child_to_nephew[0], &ok, sizeof(char));
                    
                }

                printf("nipote termina (%d = %c)\n", nbuf, nbuf);

                exit(nbuf);

            }else{
                
                // figlio
                close(child_to_nephew[0]);
                close(nephew_to_child[1]);
                char* child_file = argv[i + 1];
                int fd_child = open(child_file, O_RDONLY);
                char buf;
                
                if(fd_child < 0) exit(-1);

                while(read(fd_child, &buf, sizeof(char))){
                    
                    printf("figlio %i legge carattere %c\n", i, buf);
                    write(merge, &buf, sizeof(char));
                    write(child_to_nephew[1], &ok, sizeof(char));
                    read(nephew_to_child[0], &ok, sizeof(char));

                }

                char end = '\0';
                write(child_to_nephew[1], &end, sizeof(char)); // figlio ha finito, quindi anche il nipote (file stessa lunghezza)

                int status;
                if(wait(&status) < 0) exit(-1);

                exit(WEXITSTATUS(status));

            }

        }

    }

    for(int i = 0; i < children_number; i++){

        int status, pid;

        if((pid = wait(&status)) < 0){
            printf("errore in wait\n");
            exit(2);
        }

        status = WEXITSTATUS(status);
        printf("%d ha ritornato %c\n", pid, status);

    }

}