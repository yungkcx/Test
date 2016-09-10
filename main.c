#include "bin.h"

#define MAX 100

int main(int argc, char **argv)
{
	int listenfd, connfd, n;
	char buf[MAXLINE];

	memset(buf, 0, MAXLINE);
	listenfd = tcp_listen("localhost", "33333", NULL);
	connfd = h_accept(listenfd, NULL, NULL);
	n = MAXLINE;
	while (1) {
		memset(buf, 0, n);
		n = readn(connfd, buf, MAXLINE);
		if (n == 0) {
			break;
		}
		puts(buf);
	}
	return 0;
}
