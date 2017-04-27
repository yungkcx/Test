CC=gcc
CFLAGS= -Wall -std=gnu11 -L. -g
CSTATICFLAGS= ${CFLAGS} -static-libgcc -static
TESTLIB=libtl.a
AR=ar
ARFLAGS= rv
RM=rm -f

LIBOBJS=lib.o
STATIC_LIBOBJS=staticlib.o

all: ${LIBOBJS}
	${AR} ${ARFLAGS} ../${TESTLIB} $^

${LIBOBJS}: lib.c
	${CC} ${CFLAGS} -c $^

static: ${STATIC_LIBOBJS}
	${AR} ${ARFLAGS} ../${TESTLIB} $^

${STATIC_LIBOBJS}: lib.c
	${CC} ${CSTATICFLAGS} -o $@ -c $^

.PHONY: clean
clean:
	${RM} *.o
