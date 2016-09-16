#include "bin.h"

#define MAX 100

void sig_alrm(int signo)
{
	return;
}

int main()
{
	int listenfd, connfd, n;
	char buf[MAX];
	struct sockaddr cli;
	socklen_t len;
	struct sigaction sa;

	sa.sa_flags = 0;
	sa.sa_handler = sig_alrm;
	sigaction(SIGALRM, &sa, NULL);
	listenfd = tcp_connect("localhost", PORT);
	alarm(1);
	recv(listenfd, buf, MAX, 0);
	if (errno == EINTR)
		perror("eintr");

	return 0;
}
