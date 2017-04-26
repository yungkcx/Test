include ../Makefile.define

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
