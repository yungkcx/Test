#include "all.h"

void sig_handler(int);
char wdbuf[MAXPATHLEN];

int main(int argc, char **argv)
{
    printf("%lu\n", SSIZE_MAX);

    exit(0);
}

void sig_handler(int signo)
{
    switch (signo) {
    case SIGINT:
        printf("Interrupt\n%s%% ", getcwd(wdbuf, MAXPATHLEN));
        fflush(stdout);
        break;
    default:
        ;
    }
}
