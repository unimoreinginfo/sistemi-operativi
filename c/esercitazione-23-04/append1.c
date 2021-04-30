
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PERM 0644

int appendfile(char *f1)
{
    int outfile, nread;
    char buffer [BUFSIZ];
    if ((outfile = open( f1, O_WRONLY | O_APPEND)) < 0) {
        if ((outfile = creat(f1, PERM)) < 0)
            return 1;
    }
    else
        lseek (outfile, 0L, 2);
    /* se il file esiste, ci si posiziona alla fine */
    while ((nread = read(0, buffer, BUFSIZ)) > 0) {
        if (write (outfile, buffer, nread ) < nread) {
            close(outfile);
            return 2; /* errore */ 
        }
    } /* fine del file di input */
    close(outfile);
    return 0;
}

int main(int argc, char** argv)
{
    int integi;
    if (argc != 2) /* controllo sul numero di argomenti */ {
        printf ("ERRORE: ci vuole un argomento\n"); 
        exit(3);
    }
    integi = appendfile(argv[1]);
    exit(integi);
}
