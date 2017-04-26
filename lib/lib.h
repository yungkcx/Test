#ifndef LIB_H__
# define LIB_H__

#include <sys/socket.h>

unsigned parse_hex4(const char *p);
void encode_utf8(char *c, unsigned u);
unsigned decode_utf8(char* ch);

struct addrinfo *host_serv(const char *host, const char *port, int family, int socktype);
const char *sock_ntop(const struct sockaddr *sa);
int tcp_listen(const char *host, const char *port, socklen_t *addrlenp);
int tcp_connect(const char *host, const char *port);
int udp_client(const char *host, const char *port, struct sockaddr **saptr, socklen_t *len);
int udp_connect(const char *host, const char *port);
int udp_server(const char *host, const char *port, socklen_t *addrlenp);

int accept_e(int sockfd, struct sockaddr *addr, socklen_t *addrlenp);
ssize_t writen(int fd, const void *buf, size_t n);
ssize_t readn(int fd, void *buf, size_t n);

void eret(const char *fmt, ...);
void esys(const char *fmt, ...);
void emsg(const char *fmt, ...);
void equit(const char *fmt, ...);

int set_fl(int fd, int flags);
int clr_fl(int fd, int flags);
void pr_mask(const char*);

#endif /* LIB_H__ */
