#include "bin.h"

#define MAX 100

void sig_alrm(int signo)
{
	return;
}

int main()
{
	int listenfd, n;
	char buf[MAX];
	struct sockaddr cli;
	socklen_t len;

	vfork();
	while(1);

	return 0;
}
