CC = gcc
CFLAGS = -Wall -g -O2
shell: main.o shell.o tokenizer.o
	$(CC) $(CFLAGS) main.o shell.o tokenizer.o -o shell

main.o:	main.c
	$(CC) $(CFLAGS) -c main.c
shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c
tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c
clean: 
	rm shell main.o shell.o tokenizer.o

