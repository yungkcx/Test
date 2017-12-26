#include "all.h"

#define MAX_EVENTS 1024

struct myevent_s
{
    int fd;
    int events;
    void *args;
    void (*callback)(int fd, int events, void *args);
    int status;
    char buf[BUFLEN];
    int len;
    long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENTS + 1];

void eventset(struct myevent_s *ev, int fd, void (*callback)(int, int, void *), void *args);
void recvdata(int fd, int events, void *args);
void senddata(int fd, int events, void *args);
void eventadd(int efd, int events, struct myevent_s *ev);
void eventdel(int efd, struct myevent_s *ev);
void acceptconn(int lfd, int events, void *args);
void initlistensocket(int efd, const char *port);

int main(int argc, char **argv)
{
    const char *port = PORT;
    if (argc == 2)
        port = argv[1];

    g_efd = epoll_create(MAX_EVENTS + 1);
    if (g_efd <= 0)
        debug("create efd error");

    initlistensocket(g_efd, port);

    struct epoll_event events[MAX_EVENTS + 1];
    debug("server running: port[%s]", port);
    int checkpos = 0;

    while (1)
    {
        long now = time(NULL);
        for (int i = 0; i < 100; i++, checkpos++)
        {
            if (checkpos == MAX_EVENTS)
                checkpos = 0;
            if (g_events[checkpos].status != 1)
                continue;
            long duration = now - g_events[checkpos].last_active;
            if (duration >= 60)
            {
                close(g_events[checkpos].fd);
                debug("[fd=%d] timeout", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);
            }
        }

        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if (nfd < 0)
            equit("epoll_wait error");
        for (int i = 0; i < nfd; i++)
        {
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
            {
                ev->callback(ev->fd, events[i].events, ev->args);
            }
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                ev->callback(ev->fd, events[i].events, ev->args);
            }
        }
    }
}

void eventset(struct myevent_s *ev, int fd, void (*callback)(int, int, void *), void *args)
{
    ev->fd = fd;
    ev->callback = callback;
    ev->events = 0;
    ev->args = args;
    ev->status = 0;
    ev->last_active = time(NULL);
}

void recvdata(int fd, int events, void *args)
{
    struct myevent_s *ev = (struct myevent_s *)args;
    int len;

    len = recv(fd, ev->buf, sizeof(ev->buf), 0);
    eventdel(g_efd, ev);

    if (len > 0)
    {
        ev->len = len;
        ev->buf[len] = '\0';
        debug("C[%d]:%s", fd, ev->buf);
        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    }
    else if (len == 0)
    {
        close(ev->fd);
        debug("[fd=%d] pos[%d], closed", fd, (int)(ev - g_events));
    }
    else
    {
        close(ev->fd);
        esys("recv[fd=%d] error", fd);
    }
}

void senddata(int fd, int events, void *args)
{
    struct myevent_s *ev = (struct myevent_s *)args;
    int len;

    len = send(fd, ev->buf, ev->len, 0);

    eventdel(g_efd, ev);
    if (len > 0)
    {
        debug("send[fd=%d], [%d]%s", fd, len, ev->buf);
        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);
    }
    else
    {
        close(ev->fd);
        esys("send[fd=%d] error", fd);
    }
}

void eventadd(int efd, int events, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if (ev->status == 1)
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if (epoll_ctl(efd, op, ev->fd, &epv) < 0)
    {
        debug("event add failed [fd=%d], events[%d]", ev->fd, events);
    }
    else
    {
        debug("event add OK [fd=%d], op=%d, events[%0x]", ev->fd, op, events);
    }
}

void eventdel(int efd, struct myevent_s *ev)
{
    struct epoll_event epv = {0, {0}};

    if (ev->status != 1)
        return;

    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
}

void acceptconn(int lfd, int events, void *args)
{
    struct sockaddr cin;
    socklen_t len = sizeof(cin);
    int cfd;

    if ((cfd = accept_e(lfd, &cin, &len)) == -1)
    {
        if (errno != EAGAIN)
        {
            // Do nothing.
        }
        handle_err(errno, "accept error");
    }

    int i;
    do
    {
        for (i = 0; i < MAX_EVENTS; i++)
        {
            if (g_events[i].status == 0)
                break;
        }
        if (i == MAX_EVENTS)
        {
            debug("max connect limit[%d]", MAX_EVENTS);
            break;
        }

        int flag = 0;
        if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0)
        {
            debug("fcntl O_NONBLOCK error: %s", strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    } while (0);
    debug("new connect [%s][time:%ld], pos[%d]", sock_ntop(&cin), g_events[i].last_active, i);
}

void initlistensocket(int efd, const char *port)
{
    int lfd;

    lfd = tcp_listen("0.0.0.0", port, NULL);
    set_fl(lfd, O_NONBLOCK);
    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);
}
