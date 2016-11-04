#ifndef BIN_H
#	define BIN_H

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

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

#define SA struct sockaddr

#define MAXLINE 1024
#define PORT    "33333"
#define LISTENQ 10
#define PATH    "/tmp/gittest"

#define UNUSED(v)            (void)(v);
#define max(a, b) ({\
		typeof(a) _max1 = (a);\
		typeof(b) _max2 = (b);\
		_max1 > _max2 ? _max1 : _max2;})
#define array(T, N)          typeof(T [N])
#define STRCMP(a, R, b)      (strcmp(a, b) R 0)
#define STRNCMP(a, R, b, n)  (strncmp(a, b, n) R 0)
#define debug(M, ...)        fprintf(stderr, "DEBUG %s (in function '%s':%d: " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

extern unsigned parse_hex4(const char *p);
extern void encode_utf8(char *c, unsigned u);

extern struct addrinfo *host_serv(const char *host, const char *port, int family, int socktype);
extern const char *sock_ntop(const struct sockaddr *sa);
extern int tcp_listen(const char *host, const char *port, socklen_t *addrlenp);
extern int tcp_connect(const char *host, const char *port);
extern int udp_client(const char *host, const char *port, struct sockaddr **saptr, socklen_t *len);
extern int udp_connect(const char *host, const char *port);
extern int udp_server(const char *host, const char *port, socklen_t *addrlenp);

extern int accept_e(int sockfd, struct sockaddr *addr, socklen_t *addrlenp);
extern ssize_t writen(int fd, const void *buf, size_t n);
extern ssize_t readn(int fd, void *buf, size_t n);

extern void eret(const char *fmt, ...);
extern void esys(const char *fmt, ...);
extern void emsg(const char *fmt, ...);
extern void equit(const char *fmt, ...);

extern int set_fl(int fd, int flags);
extern int clr_fl(int fd, int flags);

#endif
