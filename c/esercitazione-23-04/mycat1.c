
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF_SZ 128

int main(int argc, char* argv[])
{
    char buf[MAX_BUF_SZ];

    if (argc == 0)
    {
        while (1)
        {
            ssize_t size = read(STDIN_FILENO, buf, MAX_BUF_SZ);
            write(STDOUT_FILENO, buf, size);
        }
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            int f = open(argv[i], O_RDONLY);
            if (f < 0)
            {
                printf("mycat1: file non esistente: %s\n", argv[i]);
                continue;
            }

            do
            {
                ssize_t size = read(f, buf, MAX_BUF_SZ);
                write(STDOUT_FILENO, buf, size);
                if (size < MAX_BUF_SZ) {
                    break;
                }
            } while (1);
        }
    }

    return 0;
}

