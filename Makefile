CC = g++
CFLAGS =  -O3 -Wall -pedantic
GLFLAGS = -lglut -lGL -lGLU -lm


ohjelma: main.cc alusta.h
	${CC}  ${CFLAGS} -o $@ $^ ${GLFLAGS}

clean:
	/bin/rm *.o
