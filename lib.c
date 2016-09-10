#include "bin.h"

/* handle SIGINTR of accept*/
int h_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int connfd;

	while (1) {
		if ((connfd = accept(sockfd, addr, addrlen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				perror("h_accept error");
		} else {
			return connfd;
		}
	}
}

ssize_t readn(int fd, void *buf, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = (char *) buf;
	nleft = n;
	while (1) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR) {
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
		error(EXIT_FAILURE, 0, "tcp_listen error for %s, %s", host, port);
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
			break;
		if (close(fd) < 0)
			perror("close error");
	} while ((res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, 0, "tcp_connect error for %s, %s", host, port);

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
		error(EXIT_FAILURE, 0, "udp_connect error for %s, %s: %s", host, port, gai_strerror(n));
	ressave = res;

	do {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd >= 0)     /* Success */
			break;
	} while ((res = res->ai_next) != NULL);
	if (res == NULL)
		error(EXIT_FAILURE, 0, "udp_client error for %s, %s", host, port);

	*saptr = (struct sockaddr *)malloc(res->ai_addrlen);
	if (*saptr == NULL)
		error(EXIT_FAILURE, 0, "malloc error");
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*len = res->ai_addrlen;

	freeaddrinfo(ressave);
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
		error(EXIT_FAILURE, 0, "tcp_listen error for %s, %s", host, port);
	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd;
}
