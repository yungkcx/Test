CC=gcc
CFLAGS= -Wall -std=gnu11 -L. -g
CSTATICFLAGS= ${CFLAGS} -static-libgcc -static
LIBS= -lpthread -lzmq -lm
TESTLIB=libtl.a
TESTPROGRAM=test statictest
LIBDIR=lib
AR=ar
ARFLAGS= rv
RM=rm -f

all: main.o testlib
	${CC} $< ${CFLAGS} ${LIBS} ${TESTLIB}

test: test.o testlib
	${CC} $< -o $@ ${CFLAGS} ${LIBS}

testlib:
	cd ${LIBDIR} && make

static: main.o statictestlib
	${CC} $< ${CSTATICFLAGS} ${LIBS}

statictest: test.o statictestlib
	${CC} $< -o $@ ${CSTATICFLAGS} ${LIBS}

statictestlib:
	cd ${LIBDIR} && make static

.PHONY: clean
clean:
	cd ${LIBDIR} && make clean
	${RM} *.o *.out ${TESTLIB} ${TESTPROGRAM}
