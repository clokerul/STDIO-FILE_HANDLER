CC = gcc
MAIN = main
EXE = main

build: main.o
	$(CC) -Wall -o $(EXE) $(MAIN).o so_stdio_impl.c

main.o: $(MAIN).c
so_stdio.o: so_stdio.c