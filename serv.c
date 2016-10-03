#include "all.h"

#define MAXBUF 10

int main(int argc, char **argv)
{
    int fd, n;
    char buf[MAXBUF];

    fd = open("txt", O_TRUNC | O_CREAT | O_WRONLY, 0664);
    if (fd < 0)
        errsys("open error");
    while ((n = read(STDIN_FILENO, buf, MAXBUF)) == MAXBUF)
        write(fd, buf, n);
    if (n < 0)
        errsys("read error");
    else
        write(fd, buf, n);

    return 0;
}
