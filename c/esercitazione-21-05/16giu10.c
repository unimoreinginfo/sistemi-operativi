#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];

typedef struct {
	int c1;
	int c2;
} strct;

/* readline --> var globale che serve per scrivere o meno la linea su stdout */
bool readline;

void handlerSig1(int sig){
	readline = true;
}

void handlerSig2(int sig){
	readline = false;
}

int main(int argc, char const *argv[]){

	/* SIGUSR1 --> ordino al figlio di scrivere la linea */
	signal(SIGUSR1, handlerSig1);
	/* SIGUSR2 --> ordino al figlio di non scrivere la linea */
	signal(SIGUSR2, handlerSig2);
	
	int N = argc - 2;
	if(N < 2){
		printf("errore nel numero di parametri\n");
		exit(1);
	}

	int H = atoi(argv[N + 1]);
	if(H <= 0 || H > 255){
		printf("errore nel parametro H\n");
		exit(2);
	}

	/* ogni figlio legge dalla pipe [i-1] e scrive sulla pipe [i] */
	piped_t* piped;
	piped = (piped_t*) malloc(N * sizeof(piped_t));
	if(piped == NULL){
		printf("errore nella creazione delle pipe\n");
		exit(3); 
	}
	for(int i = 0; i < N; i++){
		if(pipe(piped[i]) < 0){
			printf("errore nella crazione delle pipe\n");
			exit(4);
		}
	}

	int* pid = (int*) malloc(N * sizeof(int));
	for(int i = 0; i < N; i++){
		if((pid[i] = fork()) < 0){
			printf("errore nella creazione di un processo figlio\n");
			exit(5);
		}
		if(pid[i] == 0){
			printf("[%d] figlio %d associato al file %s\n", i, getpid(), argv[i + 1]);

			/* chiusura pipe */
			/* leggo i dati da piped[i-1][0] */
			/* scrivo i dati su piped[i][1] */
			for(int j = 0; j < N; j++){
				if(j != i){
					close(piped[j][1]);
				}
				if((i == 0) || j != i - 1){
					close(piped[j][0]);
				}
			}

			int fd = open(argv[i + 1], O_RDONLY);
			if(fd < 0){
				printf("errore nell'apertura del file %s\n", argv[i + 1]);
				exit(-1);
			}

			int line_count = 0, line_lenght = 0;
			int nr, nw;
			char buffer;
			char line[255];
			while(read(fd, &buffer, sizeof(char)) > 0){
				line[line_lenght] = buffer;
				line_lenght++;
				if(buffer == '\n'){
					line[line_lenght] = '\0';
					line_count++;
					strct data, rcvdata;
					data.c1 = i;
					data.c2 = line_lenght;
					if(i != 0){
						nr = read(piped[i - 1][0], &rcvdata, sizeof(strct));
						if(nr != sizeof(strct)){
							printf("errore nella lettura dei dati sulla pipe\n");
							exit(-1);
						}
						if(rcvdata.c2 < data.c2){
							nw = write(piped[i][1], &data, sizeof(strct));
						}
						else{
							nw = write(piped[i][1], &rcvdata, sizeof(strct));
						}
						if(nw != sizeof(strct)){
							printf("errore nella scrittura dei dati sulla pipe\n");
							exit(-1);
						}	
					}
					/* se i == 0, invio direttamente la struct */
					else{
						nw = write(piped[i][1], &data, sizeof(strct));
						if(nw != sizeof(strct)){
							printf("errore nella scrittura dei dati sulla pipe\n");
							exit(-1);
						}
					}
					pause();
					//sleep(1);
					if(readline == true){
						printf("linea in questione: %s", line);
					}
					line_lenght = 0;
				}
			}

			exit(0);
		}
	}

	/* chiusura pipe */
	/* leggo i dati da piped[N-1][0] */
	/* non devo scrivere dati sulla pipe */	
	for(int i = 0; i < N; i++){
		close(piped[i][1]);
		if(i != N - 1){
			close(piped[i][0]);
		}
	}

	strct rcvdata;
	int nr;
	for(int j = 0; j < H; j++){
		nr = read(piped[N - 1][0], &rcvdata, sizeof(strct));
		if(nr != sizeof(strct)){
			printf("errore nella lettura dei dati sulla pipe\n");
			exit(6);
		}
		printf("linea numero %d:\n", j + 1);
		printf("c1: %d\n", rcvdata.c1);
		printf("c2: %d\n", rcvdata.c2);
		kill(pid[rcvdata.c1], SIGUSR1);
		for(int k = 0; k < N; k++){
			if(k != rcvdata.c1){
				kill(pid[k], SIGUSR2);
			}
		}
	}

	int pidf, status, ret;
	for(int i = 0; i < N; i++){
		pidf = wait(&status);
		if(pidf < 0){
			printf("errore nella wait\n");
			exit(7);
		}
		if((status & 0xFF) != 0){
			printf("figlio %d terminato in modo anomalo\n", pidf);
		}
		else{
			ret = (int) ((status >> 8) & 0xFF);
			printf("figlio %d ha ritornato %d (255 se errore)\n", pidf, ret);
		}
	}

	exit(0);
}
