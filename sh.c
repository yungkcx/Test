#include "all.h"

void sig_handler(int);
char wdbuf[MAXPATHLEN];

int main(int argc, char **argv)
{
    char buf[BUFSIZ];
    pid_t pid;
    int status;
    struct sigaction sa;

    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sig_handler;
    if (sigaction(SIGINT, &sa, NULL) != 0)
        esys("sigaction error");

    printf("%s%% ", getcwd(wdbuf, MAXPATHLEN));
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        if (STRCMP(buf, ==, "exit")) {
            exit(0);
        } else if (STRCMP(buf, ==, "cd")) {
            goto again;
        } else if (STRNCMP(buf, ==, "cd ", 3)) {
            if (chdir(buf + 3) < 0)
                eret("cd: %s", buf + 3);
            goto again;
        }

        if ((pid = fork()) < 0) {
            esys("fork error");
        } else if (pid == 0) {
            execlp(buf, buf, NULL);
            eret("couldn't execute: %s", buf);
            exit(127);
        }

        if ((pid = waitpid(pid, &status, 0)) < 0)
            esys("waitpid error");
again:
        printf("%s%% ", getcwd(wdbuf, MAXPATHLEN));
    }

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
