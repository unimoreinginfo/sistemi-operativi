#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv){

    srand(time(NULL));

    int pid,
        child_pid,
        status;

    if((pid = fork()) < 0){
        printf("errore in fork");
        exit(1);
    }

    if(pid == 0){
        
        exit(mia_random(100)); // exit, byte alto

    }else{

        if((child_pid = wait(&status)) < 0){
            printf("errore in wait");
            exit(2);
        }
        
        if(child_pid == pid){

            char low = (status >> 8) & 0xff;
            printf("numero casuale: %d", low);
            
        }

    }

    exit(0);

}

int mia_random(int n){
    int casuale;
    casuale = rand() % n;
    return casuale;
}