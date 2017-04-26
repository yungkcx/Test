#ifndef BIN_H
#	define BIN_H

#include <zmq.h>

#include <linux/limits.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/sockios.h>
#include <linux/netlink.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_link.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>

#include <sys/eventfd.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <net/if.h>
#include <net/ethernet.h>

#include <arpa/inet.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>

#include <termios.h>
#include <setjmp.h>
#include <err.h>
#include <inttypes.h>
#include <sys/utsname.h>
#include <wchar.h>
#include <aio.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <syslog.h>
#include <pthread.h>
#include <ctype.h>
#include <error.h>
#include <math.h>
#include <grp.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <termio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <tar.h>

#include "lib/lib.h"

#define SA struct sockaddr

#define CREATE_ARRAY(a, n, max)\
    do {\
        srand(time(NULL));\
        int i = n - 1;\
        for (; i >= 0; --i)\
            a[i] = rand() % (max);\
    } while (0)

#define CREATE_ORDER_ARRAY(a, n)\
    do {\
        int i;\
        for (i = 0; i < n; ++i)\
            a[i] = i + 1;\
    } while (0)

#define PRINT_ARRAY(a, n, format)\
    do {\
        for (int i = 0; i < n; ++i)\
            printf(format, a[i]);\
        putchar('\n');\
    } while (0)

#define SWAP(a, b)\
    do {\
        typeof(a) t = b;\
        b = a;\
        a = t;\
    } while (0)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define MAXLINE 1024
#define PORT    "33333"
#define LISTENQ 10

#define UNUSED(v)            (void)(v);
#define max(a, b) ({\
		typeof(a) _max1 = (a);\
		typeof(b) _max2 = (b);\
		_max1 > _max2 ? _max1 : _max2;})
#define array(T, N)          typeof(T [N])
#define STRCMP(a, R, b)      (strcmp(a, b) R 0)
#define STRNCMP(a, R, b, n)  (strncmp(a, b, n) R 0)
#define debug(M, ...)        fprintf(stderr, "DEBUG %s (in function '%s':%d: " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#endif
