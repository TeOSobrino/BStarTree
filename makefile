CC = gcc
INCLUDES = include/
SOURCES = source/*.c
BINARY = ./main
PROG = main.c
DFLAGS = -g3 -O3 -fsanitize=address -fstack-protector-all -fstack-clash-protection -fasynchronous-unwind-tables -D_FORTIFY_SOURCE=2
FLAGS = -g -lm -march=native
VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full -s
WFLAGS = -Wall -Wpedantic

all:
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(FLAGS) $(WFLAGS)
run: 
	@$(BINARY)
valgrind: all clear 
	valgrind $(VFLAGS) $(BINARY) 
zip:
	zip -r T3.zip main.c include source makefile
clean:
	rm $(BINARY); rm *.zip
clear:
	clear
debcompile: 
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(DFLAGS)
debug: debcompile clear
	$(BINARY)