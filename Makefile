CXX=clang++
CC=clang
CXXFLAGS= -Wall -Wno-missing-braces -Wmissing-field-initializers -std=gnu++11 -I. -g
CFLAGS= -Wall -std=gnu11 -I. -g
LIBS= -lpthread -lrt
LIBFILES= lib/*.c

main: main.c
	${CC} $^ ${LIBFILES} ${CFLAGS} ${LIBS}

cpp: main.cpp
	${CXX} $^ ${CXXFLAGS} ${LIBS}
