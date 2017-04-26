#include "all.h"

int main(int argc, char* argv[])
{
    int fd;
    char buf[BUFSIZ];
    struct linger sl;

    sl.l_onoff  = 1;
    sl.l_linger = 1;
    fd = tcp_connect("localhost", "8080");
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl)) < 0)
        esys("setsockopt error");
    for (int i = 0; i < 5; ++i) {
        send(fd, buf, BUFSIZ, 0);
        //read(fd, buf, BUFSIZ);
    }
    close(fd);
    //shutdown(fd, SHUT_RDWR);
}
