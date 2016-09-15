#ifndef MYUNP_H
#	define MYUNP_H

const char *sock_ntop(const struct sockaddr *sa, socklen_t len);

int accept_e(int sockfd, struct sockaddr *addr, socklen_t *addrlenp);

ssize_t writen(int fd, const void *buf, size_t n);

ssize_t readn(int fd, void *buf, size_t n);

int tcp_listen(const char *host, const char *port, socklen_t *addrlenp);

int tcp_connect(const char *host, const char *port);

int udp_client(const char *host, const char *port, struct sockaddr **saptr, socklen_t *len);

int udp_connect(const char *host, const char *port);

int udp_server(const char *host, const char *port, socklen_t *addrlenp);

#endif
