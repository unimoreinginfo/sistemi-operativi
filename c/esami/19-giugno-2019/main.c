/*

    nota: la traccia è molto complicata
    sostanzialmente ogni figlio deve leggere il proprio file e, per ogni linea, mandare al padre il primo carattere [...]

    il problema sorge dal fatto che ogni file ha una lunghezza "dinamica", nel senso che i file non hanno tutti lo stesso numero di linee.
    pertanto al padre viene inviato OGNI VOLTA un array contente:
        a. il primo carattere della linea
        b. se il figlio ha finito di leggere le proprie linee viene messo il carattere '\0' per indicare che il processo ha terminato la sua computazione

    nel padre viene fatto un ciclo che ogni volta "raccoglie" tutti i caratteri inviati sulla pipe (tramite la funzione receive¹)
    poiché il numero di valori raccolti NON È FISSATO, poiché un figlio può terminare il lavoro prima di altri, è necessario ciclare all'interno dell'array ricevuto
    e verificare che esso contenga solo valori VALIDI (ovvero diversi da '0'), a indicare quindi che ci sono ancora figli operativi (poiché non hanno ancora inviato il carattere '0')

    fintanto che esistono valori validi viene dunque notificato il figlio con indice 
    corrispondente al valore massimo inviato e tutto continua finché l'array ricevuto non sarà composto da solo '\0'

    NOTA BENE: ho usato '\0' come valore di "fine" dato che sono sicuro che una linea non potrà mai cominciare col terminatore (dato che il file a quel punto sarebbe già terminato)


*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

typedef int piped_t[2];
int search_max_index(char*, int);
void print(char* arr, int len);
bool is_empty(char* items, int len);
void do_nothing(int index, int* array, int len);
char* receive(piped_t* pipes, int len, int* blacklist);

int readline = 0;

void h1(int sig){
    // 4 se readline è 1 allora il figlio stampa la sua linea
    readline = 1;
}
void h2(int sig){
    // ³ se readline è 0 allora il figlio non stamperà la sua linea
    readline = 0;
}

int main(int argc, char** argv){

    signal(SIGUSR1, h1);
    signal(SIGUSR2, h2);

    int N = argc - 1;
    char sync = 'x';
    if(N < 2){

        printf("errore nel numero dei caratteri\n");
        exit(1);

    }

    piped_t* pipes = malloc(sizeof(piped_t) * N);
    int* blacklist = malloc(sizeof(int) * N);
    int* children = malloc(sizeof(int) * N);

    if(!pipes || !blacklist){
        printf("errore nella malloc delle pipe");
        exit(2);
    }

    for(int i = 0; i < N; i++){
        blacklist[i] = -1;    
    }

    for(int i = 0; i < N; i++){

        if(pipe(pipes[i]) != 0){

            printf("errore nella generazione delle pipe");
            exit(1);

        }

    }

    for(int i = 0; i < N; i++){

        char* F = argv[i + 1];
        int P;

        if((P = fork()) < 0){

            printf("errore in fork\n");
            exit(2);

        }

        children[i] = P;

        if(P == 0){
            
            for(int j = 0; j < N; j++){

                close(pipes[j][0]);
                if(j != i) close(pipes[j][1]);

            }

            int fd = open(F, O_RDONLY),
            occ = 0;
            if(fd < 0) exit(-1);

            char line[250];
            int c = 0, lines = 0;

            while(read(fd, &line[c], sizeof(char)) > 0){

                if(line[c] == '\n'){
                    
                    lines++;
                    line[c + 1] = 0;
                    write(pipes[i][1], &line[0], sizeof(char));
                    pause();

                    if(readline){ // readline viene "togglata" nell'handler (annotazione 4 in cima al file)
                        printf("%d (indice %d) stampa: %c, %s\n", getpid(), i, line[0], line);
                        readline = 0;
                    }

                    line[0] = 0;
                    c = 0;

                } else c++;

            }

            // il figlio ha finito di scrivere le sue linee
            char end = '\0';
            write(pipes[i][1], &end, sizeof(char));
            exit(lines);

        }

    }

    int line = 1;
    char* characters = receive(pipes, N, blacklist);

    while(!is_empty(characters, N)){

        printf("linea %d riceve la seguente lista di caratteri (spazio vuoto = figlio ha finito di leggere):\n", line);
        int max_idx = search_max_index(characters, N);
        print(characters, N);
        printf("\n");
        printf("il figlio di indice %d stamperà la sua linea\n", max_idx);

        kill(children[max_idx], SIGUSR1); // dico al figlio in questione di svegliarsi e stampare la linea (annotazione 4)
        do_nothing(max_idx, children, N); // annotazione ²

        characters = receive(pipes, N, blacklist);
        line++;

    }

    for(int i = 0; i < N; i++){

        int p, status;
        if((p = wait(&status)) < 0){
            printf("errore in wait");
            exit(2);
        }

        status = WEXITSTATUS(status);
        printf("figlio con pid %d ha ritornato %d\n", p, status);

    }
    
}

void print(char* arr, int len){

    printf("[ ");
    for(int i = 0; i < len; i++){

        printf("%c ", arr[i]);

    }
    printf("]");

}

bool is_empty(char* items, int len){

    for(int i = 0; i < len; i++){

        if(items[i] != '\0')
            return false;

    }

    return true;

}

char* receive(piped_t* pipes, int len, int* blacklist){

    /*

        ¹ 
        la funzione receive legge da tutte quante le pipe dei figli
        finché un figlio non invia il carattere '\0' viene considerato operativo

        quando viene inviato tale carattere il figlio viene messo in una blacklist (l'indice del figlio che ha terminato viene messo in blacklist)
        i figli nella blacklist vengono "skippati" (banalmente la blacklist è inzializzata con solo "-1" come valore iniziale)

    */

    char* items = malloc(sizeof(char) * len); // insieme lungo al max come il numero di figli
    char item;
    for(int i = 0; i < len; i++){

        if(blacklist[i] != -1) { items[i] = '\0'; continue; }; // se il figlio è blacklistato, lo skippo
        read(pipes[i][0], &item, sizeof(char));

        if(item == '\0') blacklist[i] = i;  // il figlio ha inviato '\0', pertanto ha finito e quindi posso skipparlo
        
        items[i] = item;
        
    }

    return items;

}

int search_max_index(char* array, int len){

    int index;
    for(int i = 0; i < len; i++){

        if(i == 0) 
            index = 0;
        else{
            if(array[index] < array[i]) index = i;
        }

    }

    return index;

}

void do_nothing(int index, int* array, int len){

    /*
        ² 
        ciclo che dice a tutti i figli diversi da quello di indice "index" di "stare fermi"
    */

    for(int i = 0; i < len; i++){

        if(i != index) kill(array[i], SIGUSR2); // notifica tutti gli altri di non fare niente ³

    }

}