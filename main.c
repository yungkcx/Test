#include "bin.h"

#define MAX 10000

int main(int argc, char **argv)
{
    int fd, n;
    struct ifreq ifr;

    fd = udp_connect(argv[1], argv[2]);
    ioctl(fd, SIOCGIFMTU, &ifr);
    printf("%d\n", ifr.ifr_ifru.ifru_mtu);

    return 0;
}
