#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];

int search(int*, int, int);

int main(int argc, char** argv){
    
    if(argc < 4 || argc > 4){

        printf("servono esattamente 3 parametri");
        exit(1);

    }

    char* F = argv[1];
    int B = atoi(argv[2]), 
        L = atoi(argv[3]); // dimensione del file F

    if(B <= 0 || L <= 0){

        printf("B ed L devono essere strettamente positivi");
        exit(2);

    }

    piped_t* pipes = malloc(sizeof(piped_t) * B);
    int* children = malloc(sizeof(int) * B);

    for(int i = 0; i < B; i++){

        if(pipe(pipes[i]) < 0){
            printf("errore durante la creazione delle pipe");
            exit(3);
        }

    }

    int main_file = open(F, O_RDONLY | O_CREAT);

    if(main_file < 0){
        printf("errore durante l'apertura del file principale");
        exit(4);
    }

    for(int q = 0; q < B; q++){

        int P;

        if((P = fork()) < 0){
            printf("errore durante la generazione di un figlio");
            exit(5);
        }

        children[q] = P;

        if(P == 0){
            
            // printf("figlio (%d) con pid %d\n", q, getpid());
            for(int k = 0; k < B; k++){

                close(pipes[k][0]); // chiudo il lato di lettura
                if(k != q) close(pipes[k][1]); // chiudo il lato scrittura di tutte le pipe che non sono questa
            
            }

            off_t offset = q * L / B;
            int block_size = (((q + 1) * L) / B) - (int) offset;
            char* buff = malloc(sizeof(char) * (block_size + 1));

            lseek(main_file, 0L, SEEK_SET);
            lseek(main_file, offset, SEEK_CUR); 

            int nr = read(main_file, buff, sizeof(char) * block_size); // nr mi serve per fermarmi prima in caso il blocco sia troppo grande
            buff[nr] = 0;
            char last_char = buff[nr - 1];
            // printf("stringa letta da %d: %s\n", q, buff);
            write(pipes[q][1], &last_char, sizeof(char));
 
            exit(block_size);

        }

        close(pipes[q][1]); // chiudo scrittura sul padre

    }

    char filename[255];
    sprintf(filename, "%s.Chiara", F);
    creat(filename, 0644);
    int chiara_file = open(filename, O_WRONLY);
    if(chiara_file < 0){
        printf("errore durante l'apertura del file .Chiara");
        exit(10);
    }

    for(int q = 0; q < B; q++){

        char buff;
        if(read(pipes[q][0], &buff, 1) < 0) printf("errore in lettura sul figlio di indice %d\n", q);

        printf("scrivo carattere letto dal figlio di indice %d (%c)\n", q, buff);
        write(chiara_file, &buff, sizeof(char));

    }

    for(int q = 0; q < B; q++){

        int child, status;
        if((child = wait(&status)) < 0) printf("errore in lettura sul figlio di indice %d", q);

        status = WEXITSTATUS(status);
        int child_index = search(children, child, B);
        if(child_index < 0) printf("errore molto strano...");

        printf("il figlio di indice %d ha ritornato %d\n", child_index, status);


    }

}

int search(int* arr, int v, int len){
    for(int i = 0; i < len; i++){

        if(arr[i] == v)
            return i;

    }

    return -1;
}