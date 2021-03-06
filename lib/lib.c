#include "../all.h"

void pr_mask(const char* str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;
    if (sigprocmask(0, NULL, &sigset) < 0) {
        eret("sigprocmask error");
    } else {
        printf("%s", str);
        if (sigismember(&sigset, SIGINT))
            printf(" SIGINT");
        if (sigismember(&sigset, SIGQUIT))
            printf(" SIGQUIT");
        if (sigismember(&sigset, SIGUSR1))
            printf(" SIGUSR1");
        if (sigismember(&sigset, SIGALRM))
            printf(" SIGALRM");

        /* remaining signals can go here */

        puts("");
    }
    errno = errno_save;
}

int set_fl(int fd, int flags)
{
    /* Cant't be used to set O_SYNC or O_DSYNC. */
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        return val;
    val |= flags;
    return fcntl(fd, F_SETFL, val);
}

int clr_fl(int fd, int flags)
{
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        return val;
    val &= ~flags;
    return fcntl(fd, F_SETFL, val);
}

unsigned parse_hex4(const char *p)
{
    int i;
    unsigned u;

    u = 0;
    for (i = 0; i < 4; ++i) {
        u <<= 4;
        if (isdigit(p[i]))
            u += p[i] - '0';
        else if (p[i] <= 'f' && p[i] >= 'a')
            u += p[i] - 'a' + 10;
        else
            return 0;
    }
    return u;
}

unsigned decode_utf8(char* ch)
{
    unsigned u = 0;
    unsigned hex = *ch;
    if (hex <= 0xdf) {
        u |= (*ch & 0x1f);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
    } else if (hex <= 0xef) {
        u |= (*ch & 0x0f);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
    } else if (hex <= 0xf7) {
        u |= (*ch & 0x07);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
        u <<= 6, ch++;
        u |= (*ch & 0x3f);
    }
    return u;
}

void encode_utf8(char *c, unsigned u)
{
    assert(u >= 0x0000 && u <= 0x10ffff);
    if (u <= 0x007f)
        c[0] = (char) u;
    else if (u <= 0x07ff) {
        c[0] = (0xc0 | ((u >>  6) & 0xff));
        c[1] = (0x80 | ( u        & 0x3f));
    } else if (u <= 0xffff) {
        c[0] = (0xe0 | ((u >> 12) & 0xff));
        c[1] = (0x80 | ((u >>  6) & 0x3f));
        c[2] = (0x80 | ( u        & 0x3f));
    } else { /* 0x10000 ~ 0x10ffff */
        c[0] = (0xf0 | ((u >> 18) & 0xff));
        c[1] = (0x80 | ((u >> 12) & 0x3f));
        c[2] = (0x80 | ((u >>  6) & 0x3f));
        c[3] = (0x80 | ( u        & 0x3f));
    }
}

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
            str[0] = '[';
			if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
				return NULL;
			snprintf(portstr, sizeof(portstr), "]:%hu", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return str;
		}
		default:
			snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d", sa->sa_family);
			return str;
	}
	return NULL;
}

/* handle SIGINTR for accept */
int accept_e(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int connfd;

	while (1) {
		if ((connfd = accept(sockfd, addr, addrlen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				esys("accept error");
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
				esys("writen error");
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
				nread = 0;
			} else {
				esys("readn error: %d", fd);
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
			esys("setsockopt error");
		if (bind(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		if (close(fd) < 0)
			esys("close error");
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
			esys("close error");
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
			esys("close error");
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		error(EXIT_FAILURE, errno, "tcp_listen error for %s, %s", host, port);
	if (addrlenp)  /* addrlen is not necessary for UDP server. */
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return fd;
}

/* error handler functions */
static void edoit(int status, const char *fmt, va_list ap)
{
    int errno_save, n;
    char buf[MAXLINE + 1];

    errno_save = errno;
    vsnprintf(buf, MAXLINE, fmt, ap);
    n = strlen(buf);
    if (status)
        snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
    strcat(buf, "\n");
    fputs(buf, stderr);
}

void equit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    edoit(0, fmt, ap);
    va_end(ap);
    exit(EXIT_SUCCESS);
}

void esys(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    edoit(1, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void eret(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    edoit(1, fmt, ap);
    va_end(ap);
}

void emsg(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    edoit(0, fmt, ap);
    va_end(ap);
}
