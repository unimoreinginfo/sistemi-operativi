#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){

    int pid,
        child_pid,
        status,
        grep_return;

    if(argc != 3){

        printf("errore nel numero di parametri che devono essere due (stringa da cercare e nome del file dove cercare): %d\n", argc);
        exit(1);

    }

    pid = fork();

    if(pid < 0){

        printf("errore in fork\n");
        exit(2);

    }
    
    if(pid == 0){

        printf("esecuzione di grep da parte del figlio con pid %d\n", getpid());
        close(1); open("/dev/null", O_WRONLY);
        close(2); open("/dev/null", O_WRONLY);
        close(0); open(argv[2], O_RDONLY);
        execlp("grep", "grep", argv[1], (char*) 0);
        
        exit(-1);

    }
    
    child_pid = wait(&status);
    if(child_pid < 0){
        printf("errore wait\n");
        exit(3);
    }
    if((status & 0xff) != 0)
        printf("figlio con pid %d terminato in modo anomalo\n", child_pid);
    else{
        grep_return = (int)((status >> 8) & 0xff);
        printf("il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n", child_pid, grep_return);
    }

    exit(0);

}