// author: Mattia Gualtieri

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

typedef int piped_t[2];

int main(int argc, char const *argv[]){
	
	int N = argc - 2;
	if(N < 2){
		printf("errore nel numero di parametri\n");
		exit(1);
	}

	piped_t* syncpipe;
	syncpipe = (piped_t*) malloc(N * sizeof(piped_t));
	if(syncpipe == NULL){
		printf("errore nella creazione delle pipe\n");
		exit(3); 
	}
	for(int i = 0; i < N; i++){
		if(pipe(syncpipe[i]) < 0){
			printf("errore nella crazione delle pipe\n");
			exit(4);
		}
	}

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
	if(pid == NULL){
		printf("errore nella creazione dell'array di pid\n");
		exit(1);
	}

	for(int i = 0; i < N; i++){
		if((pid[i] = fork()) < 0){
			printf("errore nella creazione di un processo figlio\n");
			exit(5);
		}
		if(pid[i] == 0){
			printf("figlio %d associato al file %s\n", getpid(), argv[i + 1]);

			close(syncpipe[i][1]);	// syncpipe[i][0] <-- lettura sincronizzazione
			close(piped[i][0]); 	// piped[i][1] <-- scrittura carattere
            
			for(int j = 0; j < N; j++){
				if(j != i){
					close(syncpipe[j][0]);
					close(syncpipe[j][1]);
					close(piped[j][0]);
					close(piped[j][1]);
				}
			}

			int fd = open(argv[i + 1], O_RDONLY);
			if(fd < 0){
				printf("errore nell'apertura del file %s\n", argv[i + 1]);
				exit(-1);
			}

			int nr, nw;
			char token;
			char buffer;
			while(read(fd, &buffer, sizeof(char)) > 0){
				nr = read(syncpipe[i][0], &token, sizeof(char));
				// ricevuto il token, scrivo il carattere letto
				nw = write(piped[i][1], &buffer, sizeof(char));
			}

			exit(0);
		}
	}

	for(int i = 0; i < N; i++){
		close(syncpipe[i][0]);	// syncpipe[i][1] <-- scrittura sincronizzazione
		close(piped[i][1]); 	// piped[i][0] <-- lettura carattere
	}

	int fd = open(argv[N + 1], O_RDONLY);
	if(fd < 0){
		printf("errore nell'apertura del file %s\n", argv[N + 1]);
		exit(10);
	}

	bool* activechild = (bool*) malloc(N * sizeof(bool));
	for(int i = 0; i < N; i++){
		activechild[i] = true;
	}
	int nr, nw;
	char token = 'x';
	char buffer, rcv;
	while(read(fd, &buffer, sizeof(char)) > 0){
		for(int i = 0; i < N; i++){
			if(activechild[i] == true){
				nw = write(syncpipe[i][1], &token, sizeof(char));
				nr = read(piped[i][0], &rcv, sizeof(char));
				if(rcv != buffer){
					activechild[i] = false;
				}
			}
		}
	}

	for(int i = 0; i < N; i++){
		if(activechild[i] == false){
			kill(pid[i], SIGKILL);
		}
	}

	int pidf, status, ret;
	for(int i = 0; i < N; i++){
		pidf = wait(&status);
		if(pidf < 0){
			printf("errore nella wait\n");
			exit(6);
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