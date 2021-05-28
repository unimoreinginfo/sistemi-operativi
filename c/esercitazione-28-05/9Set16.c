#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef int piped_t[2];
typedef struct {

    char v1;
    long int v2;

} item;

void swap(item*, item*);
void bubble_sort(item*, int);

int main(int argc, char** argv){

    char* F = argv[1];
    char* alphabet = "abcdefghijklmnopqrstuvwxyz";
    piped_t *pipes = malloc(sizeof(int) * 26);
    item* global_arr = malloc(sizeof(item) * 26);
    int* children = malloc(sizeof(int) * 26);

    for(int i = 0; i < 26; i++){

        if(pipe(pipes[i]) != 0){
            printf("errore durante la creazione delle pipe");
            exit(2);
        }

    }

    for(int i = 0; i < 26; i++){

        int P;

        if((P = fork()) < 0){

            printf("errore in fork");
            exit(1);

        }

        if(P == 0){

            // printf("spawned %d\n", getpid());
            char letter = alphabet[i];
            int file = open(F, O_RDONLY);
            long int occ = 0; // occorrenze

            if(file < 0){

                printf("%d errore in apertura del file %s\n", getpid(), F);
                exit(-1);

            }

            if(i == 0)
                close(pipes[i][0]); // chiudo la lettura se è il primo figlio

            for(int j = 0; j < 26; j++){
				if(j != i){
					close(pipes[j][1]);
				}
				if((i == 0) || j != i - 1){
					close(pipes[j][0]);
				}
			}

            int read_channel = pipes[i - 1][0],
                write_channel = pipes[i][1],
                n = 0;

            char current_char;

            while(read(file, &current_char, sizeof(char)) > 0){ if(current_char == letter) occ++; }

            item sendable = { .v1 = letter, .v2 = occ };
            item* arr_to_send = malloc(sizeof(item) * 26); 

            // printf("%d ha trovato %ld occorrenze del carattere %c\n", i, sendable.v2, sendable.v1);

            if(i == 0) arr_to_send = global_arr;
            else read(read_channel, arr_to_send, sizeof(item) * 26);

            arr_to_send[i] = sendable;
            // printf("%d ha trovato %ld occorrenze del carattere %c\n", i, arr_to_send[i].v2, arr_to_send[i].v1);

            write(write_channel, arr_to_send, sizeof(item) * 26);

            exit(current_char);

        } else children[i] = P;

    }

    item* stuff = malloc(sizeof(item) * 26);
    read(pipes[25][0], stuff, sizeof(item) * 26);
    bubble_sort(stuff, 26);

    for(int i = 0; i < 26; i++){

        item current_item = stuff[i];

        printf("figlio di indice %d (pid: %d) ha trovato %ld occorrenze del carattere %c\n", i, children[i], current_item.v2, current_item.v1);

    }

    for(int i = 0; i < 26; i++){

        int child_pid, status;

        if((child_pid = wait(&status)) < 0){
            printf("errore in wait");
            exit(4);
        }

        printf("figlio con pid %d ha ritornato %d\n", child_pid, WEXITSTATUS(status));

    }

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
            if (v[i].v2 < v[i+1].v2) {
                swap(&v[i], &v[i+1]);
                ord = false;
            }
            dim--;
    }
}