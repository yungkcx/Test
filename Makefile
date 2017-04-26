include Makefile.define

all: main.o testlib
	${CC} $< ${CFLAGS} ${LIBS}

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
