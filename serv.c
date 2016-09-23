#include "bin.h"

int main(int argc, char **argv)
{
    struct sockaddr_un serv, cli;
    char buf[MAXLINE], *pathname;
    int listenfd, n;
    socklen_t len;

    if (argc >= 2)
        pathname = argv[1];
    else
        pathname = (char *) PATH;

    listenfd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (listenfd < 0)
        errsys("socket error");
    unlink(pathname);

    memset(&serv, 0, sizeof(serv));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, pathname, sizeof(serv.sun_path) - 1);

    if (bind(listenfd, (SA *) &serv, SUN_LEN(&serv)) < 0)
        errsys("bind error");

    n = MAXLINE;
    while (1) {
        len = sizeof(cli);
        memset(buf, 0, n);
        n = recvfrom(listenfd, buf, MAXLINE, 0, (SA *) &cli, &len);
        if (n < 0)
            errsys("recvfrom error");

        buf[n] = 0;
        sendto(listenfd, buf, n, 0, (SA *) &cli, len);
    }

    return 0;
}
