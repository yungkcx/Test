CC=gcc
CFLAGS= -Wall -Wno-unused -std=gnu11 -I.
LIBS= -lpthread -lrt
LIBFILES= lib/*.c

main: main.c ${LIB_DIR}
	${CC} $< ${LIBFILES} ${CFLAGS} ${LIBS} 

clean:
	rm main *.o
