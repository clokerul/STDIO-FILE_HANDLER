CC = gcc
MAIN = main
EXE = main

lib: so_stdio.o
	gcc -shared so_stdio.o -o libso_stdio.so

build: main.o
	$(CC) -Wall -o $(EXE) $(MAIN).o so_stdio.c

main.o: $(MAIN).c
so_stdio.o: so_stdio.c