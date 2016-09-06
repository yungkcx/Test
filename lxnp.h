#ifndef MYUNP_H
#	define MYUNP_H

int tcp_listen(const char *host, const char *port, socklen_t *addrlenp);

int tcp_connect(const char *host, const char *port);

int udp_client(const char *host, const char *port, struct sockaddr **saptr, socklen_t *len);

#endif
