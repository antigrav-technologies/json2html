CC=gcc
CFLAGS=-O3 -Wall -Wextra

all:
	$(CC) $(CFLAGS) main.c -o json2html