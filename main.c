#include "bin.h"

static int status;
void sighandler(int signo)
{
	switch (signo) {
		case SIGALRM:
			break;
		case SIGINT:
			break;
	case SIGCHLD:
			wait(&status);
			break;
		default:
			printf("signal %d catched\n", signo);
	}
}

typedef enum {cc, dd} e1;
typedef enum {aa1 = -1, bb} e2;

int main(int argc, char **argv)
{
	/*pid_t pid;

	if ((pid = fork()) < 0) {
		perror("fork");
	} else if (pid != 0) {
		exit(0);
	}
	//setsid();

	if (chdir("/") < 0) {
		perror("chdir");
		exit(0);
	}

	close(0);
	open("/dev/null", O_RDWR);
	dup2(0, 1);
	dup2(0, 2);
	while(1);
*/
	e1 a = (e1)(0x7fffffff + 2);
	printf("%d\n", a);
	return 0;
}
