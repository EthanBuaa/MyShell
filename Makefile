CFLAGS=-I.
DBG_FLAGS =-g -Wall

all: mysh

OBJ = shell.o history.o built_in.o

%.o: %.c %.h
	gcc -c -o $@ $< ${CFLAGS} ${DBG_FLAGS}

mysh: main.c ${OBJ}
	gcc -o $@ $^ ${CFLAGS} ${DBG_FLAGS}

.PHONY: clean
clean:
	-rm *.o 

.PHONY: valgrind
valgrind: mysh
	valgrind --leak-check=full --show-leak-kinds=all ./mysh
