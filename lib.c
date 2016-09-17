#include "bin.h"

/* return a addrinfo pointer and connect by yourself */
struct addrinfo *host_serv(const char *host, const char *port,
		int family, int socktype)
{
	int n;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME; /* canonical name */
	hints.ai_family = family;  /* AF_UNSPEC, AF_INET, etc. */
	hints.ai_socktype = socktype;  /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */
	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		return NULL;
	return res;
}

const char *sock_ntop(const struct sockaddr *sa)
{
	static char str[45]; /* X:X:X:X:X:X:a.b.c.d, so it is 45 characters */
	char portstr[8];

	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *) sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
				return NULL;
			snprintf(portstr, sizeof(portstr), ":%hu", ntohs(sin->sin_port));
			strcat(str, portstr);
			return str;
		}
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
			if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
				return NULL;
			snprintf(portstr, sizeof(portstr), ":%hu", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return str;
		}
		default:
			snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d", sa->sa_family);
			return str;
	}
	return NULL;
}

/* handle SIGINTR of accept */
int accept_e(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int connfd;

	while (1) {
		if ((connfd = accept(sockfd, addr, addrlen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				perror("lxaccept error");
		} else {
			return connfd;
		}
	}
}

ssize_t writen(int fd, const void *buf, size_t n)
{
	ssize_t nwrite;

	while (1) {
		if ((nwrite = write(fd, buf, n)) < 0) {
			if (errno == EINTR) {
				nwrite = 0;
			} else {
				perror("writen error");
				return -1;
			}
		} else {
			break;
		}
	}
	return nwrite;
}

ssize_t readn(int fd, void *buf, size_t n)
{
	ssize_t nread;

	while (1) {
		if ((nread = read(fd, buf, n)) < 0) {
			if (errno == EINTR) {
				perror("Error");
				nread = 0;
			} else {
				perror("readn error");
				return -1;
			}
		} else {
			break;
		}
	}
	return nread;
}

/* addrlenp can be NULL, it is not necessary for TCP */
int tcp_listen(const char *host,
		const char *port, socklen_t *addrlenp)
{
	int fd, n;
	struct addrinfo hints, *res, *ressave;
	const int on = 1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		error(EXIT_FAILURE, 0, "tcp_listen error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			perror("setsockopt error");
		if (bind(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		if (close(fd) < 0)
			perror("close error");
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, errno, "tcp_listen error for %s, %s", host, port);
	if (listen(fd, LISTENQ) < 0)
		error(EXIT_FAILURE, errno, "listen error");
	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd;
}

int tcp_connect(const char *host, const char *port)
{
	int fd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		error(EXIT_FAILURE, 0, "tcp_connect error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;
		if (connect(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;         /* Success */
		if (close(fd) < 0)
			perror("close error");
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, errno, "tcp_connect error for %s, %s", host, port);

	freeaddrinfo(ressave);

	return fd;
}

/* saptr and len will be used in sendto(), and len cannot be NULL
 * because sendto() and recvfrom() always need it */
int udp_client(const char *host, const char *port,
		struct sockaddr **saptr, socklen_t *len)
{
	int fd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		error(EXIT_FAILURE, 0, "udp_client error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd >= 0)     /* Success */
			break;
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, errno, "udp_client error for %s, %s", host, port);

	*saptr = (struct sockaddr *)malloc(res->ai_addrlen);
	if (*saptr == NULL)
		error(EXIT_FAILURE, errno, "malloc error");
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*len = res->ai_addrlen;

	freeaddrinfo(ressave);
	return fd;
}

int udp_connect(const char *host, const char *port)
{
	int fd, n;
	socklen_t len;
	struct sockaddr *cli;

	fd = udp_client(host, port, &cli, &len);
	n = connect(fd, cli, len);
	free(cli);
	if (n < 0)
		return -1;
	else
		return fd;
}

int udp_server(const char *host,
		const char *port, socklen_t *addrlenp)
{
	int fd, n;
	struct addrinfo hints, *res, *ressave;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(host, port, &hints, &res)) != 0)
		error(EXIT_FAILURE, 0, "udp_server error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;
		/* do not need SO_REUSEADDR because UDP have not TIME_WAIT */
		if (bind(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		if (close(fd) < 0)
			perror("close error");
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, errno, "tcp_listen error for %s, %s", host, port);
	if (addrlenp)  /* addrlen is not necessary for UDP server. */
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd;
}
