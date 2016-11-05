CC=clang
CFLAGS= -Wall -Wno-unused -std=gnu11 -I. -g
LIBS= -lpthread -lrt
LIBFILES= lib/*.c

main: main.c ${LIB_DIR}
	${CC} $< ${LIBFILES} ${CFLAGS} ${LIBS} 

clean:
	rm main *.o
