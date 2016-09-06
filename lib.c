#include "bin.h"

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
