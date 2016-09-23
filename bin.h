#ifndef BIN_H
#	define BIN_H

#include <sys/un.h>
#include <linux/if_link.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <ifaddrs.h>
#include <syslog.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/wait.h>
#include <ctype.h>
#include <error.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <termio.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdarg.h>
#include "lxnp.h"

#define SA struct sockaddr

#define MAXLINE 1024
#define PORT    "33333"
#define LISTENQ 10
#define PATH    "/tmp/gittest"

#define UNUSED(...) (void)(__VA_ARGS__)
#define max(a, b) ({\
		typeof(a) _max1 = (a);\
		typeof(b) _max2 = (b);\
		_max1 > _max2 ? _max1 : _max2;})

#define array(T, N) typeof(T [N])

#endif
