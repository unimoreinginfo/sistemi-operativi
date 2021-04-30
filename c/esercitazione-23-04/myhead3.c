

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc > (2 + 1))
    {
        printf("Sintassi invalida: %s [-N] [F]\n", argv[0]);
        exit(1);
    }

    int fd;
    int n;

    if (argc <= 1)
    {
        n = 10;
    }
    else if (argc >= (1 + 1)) // cmd -N ...
    {
        argv[1]++; // skippa il carattere -
        n = atoi(argv[1]);
        if (n == 0)
        {
            printf("Numero non valido: %s\n", argv[1]);
            exit(2);
        }

        if (argc >= (2 + 1)) // cmd -N F ...
        {
            fd = open(argv[2], O_RDONLY);
        }
        else
        {
            fd = STDIN_FILENO;
        }
    }

    size_t i = 0;
    size_t lines_cnt = 0;

    char buf[128];
    while (1)
    {
        char c;    
        ssize_t ret = read(fd, &c, sizeof(char));
        buf[i] = c;
    
        if (c == '\n')
        {
            buf[i] = '\0';
            puts(buf);
            i = 0;

            lines_cnt++;
            if (lines_cnt >= n)
            {
                break;
            }
        }

        i++;

        if (ret < 0) {
            break;
        }
    }

    return 0;
}
