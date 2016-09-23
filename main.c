#include "bin.h"

int main(int argc, char **argv)
{
    struct sockaddr_un serv;
    char buf[MAXLINE], *pathname;
    int fd, n;
    socklen_t len;

    if (argc >= 2)
        pathname = argv[1];
    else
        pathname = (char *) PATH;

    fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (fd < 0)
        errsys("socket error");

    memset(&serv, 0, sizeof(struct sockaddr_un));
    serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, pathname, sizeof(serv.sun_path));

    memset(buf, 0, MAXLINE);
    len = sizeof(serv);
    strncpy(buf, argv[2] ? argv[2] : "okok", argv[2] ? strlen(argv[2]) : 4);
    sendto(fd, buf, strlen(buf), 0, (SA *) &serv, len);
    n = recvfrom(fd, buf, MAXLINE, 0, (SA *) &serv, &len);
    if (n < 0)
        errsys("received EOF");

    buf[n] = 0;
    printf("received %d bytes: %s\n", n, buf);

    return 0;
}
