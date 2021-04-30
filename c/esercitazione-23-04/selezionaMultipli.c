


#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != (2 + 1))
    {
        printf("Sintassi invalida: %s <F> <n>\n", argv[0]);
        exit(1);
    }

    // controllo file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("File non valido: %s\n", argv[1]);
        exit(2);
    }

    // controllo numero intero strettamente positivo
    int n = atoi(argv[2]);
    if (n == 0)
    {
        printf("Numero non valido: %s\n", argv[2]);
        exit(3);
    }

    char* buf = (char*) malloc(sizeof(char) * n);
    int mul = 0;

    do
    {
        ssize_t size = read(fd, buf, sizeof(char) * n);
        if (size < n) {
            break;
        }

        printf("Il char %d-esimo del file e': %c\n", mul, buf[n - 1]);

        mul++;

    } while (1);

    free(buf);

    return 0;
}
