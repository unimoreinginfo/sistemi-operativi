#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

typedef int piped_t[2];
typedef struct {
    int c1;
    int c2;
} item;

void swap(item*, item*);
void bubble_sort(item*, int);

void print_stuff(item* arr, int len, char** argv);

int main(int argc, char** argv){

    int N = argc - 1;
    int file_len = atoi(argv[N]);
    char ok = 'k';
    
    if(N < 2){
        printf("N dev'essere maggiore o uguale a 2\n");
        exit(1);
    }

    if(file_len <= 0){

        printf("la lunghezza dei file deve essere strettamente positiva!\n");
        exit(1);

    }

    piped_t* pipeline = (piped_t*) malloc(sizeof(piped_t));
    if(!pipeline){
        printf("errore nell'allocazione della pipeline\n");
        exit(2);
    }

    for(int i = 0; i < N; i++){

        if(pipe(pipeline[i]) != 0){
            printf("errore nella generazione delle pipe\n");
            exit(2);
        }

    }

    for(int i = 0; i < N - 1; i++){

        int P = fork();

        if(P < 0){
            printf("errore in fork\n");
            exit(2);
        }

        if(P == 0){

            char* filename = argv[i + 1];
            int fd = open(filename, O_RDONLY);

            if(fd < 0){
                printf("errore in apertura del file %s\n", filename);
                exit(-1);
            }

            int read_channel = pipeline[i - 1][0],
                write_channel = pipeline[i][1];

            if(i == 0) close(pipeline[i][0]);

            for(int q = 0; q < N - 1; q++){

                if(q != i) close(pipeline[q][1]);
                if(q != i - 1) close(pipeline[q][0]);

            }

            char buf;
            item* send, *recv;
            int len = 1; // calcolare anche terminatore

            
            while(read(fd, &buf, sizeof(char))){

                if(buf == '\n'){

                    if(i == 0){

                        // non devo leggere da nessuna parte
                        send = (item*) malloc(sizeof(item));
                        send[0].c1 = getpid();
                        send[0].c2 = len;
                        write(write_channel, send, sizeof(item));

                    }else{

                        int rec_bytes = sizeof(item) * (i);
                        recv = (item*) malloc(rec_bytes);

                        read(read_channel, recv, rec_bytes);
                        send = (item*) malloc(rec_bytes + sizeof(item));
                        memcpy(send, recv, rec_bytes + sizeof(item));
                        send[i].c1 = getpid(),
                        send[i].c2 = len;
                        write(write_channel, send, rec_bytes + sizeof(item));
                    }

                    len = 1;

                } else len++;

            }

            exit(i);

        }

    }

    item* arr = malloc(sizeof(item) * file_len);

    for(int i = 0; i < file_len; i++){

        printf("linea %d \n", i + 1);
        read(pipeline[N - 2][0], arr, sizeof(item) * file_len);
        bubble_sort(arr, N - 1);
        print_stuff(arr, N - 1, argv);

    }

    for(int i = 0; i < N - 1; i++){

        int status, pid;
        if((pid = wait(&status)) < 0){
            printf("errore in wait\n");
            exit(2);
        }

        printf("%d ha ritornato %d\n", pid, WEXITSTATUS(status));

    }

}

void print_stuff(item* arr, int len, char** argv){

    for(int i = 0; i < len; i++){

        printf("c1: %d, c2: %d, file: %s, figlio: %d\n", arr[i].c1, arr[i].c2, argv[i + 1], i);

    }

    printf("\n");

}

void swap(item* v1, item* v2){
    item tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}
 
void bubble_sort(item v[], int dim){
    int i; 
    bool ord = false;
    while (dim > 1 && !ord) { 
        ord = true;
        for (i = 0; i < dim - 1; i++)
            if (v[i].c2 < v[i+1].c2) {
                swap(&v[i], &v[i+1]);
                ord = false;
            }
            dim--;
    }
}