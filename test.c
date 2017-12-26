#include "all.h"

int main()
{
    int fd;
    char buf[MAXLINE];

    fd = tcp_connect("localhost", PORT);
    strcpy(buf, "Hello, world!\n");
    write(fd, buf, strlen(buf));
    read(fd, buf, MAXLINE);
    write(fd, buf, strlen(buf));
    read(fd, buf, MAXLINE);
    write(fd, buf, strlen(buf));
    read(fd, buf, MAXLINE);
    write(fd, buf, strlen(buf));
    read(fd, buf, MAXLINE);
    printf("read: %s", buf);

    return 0;
}