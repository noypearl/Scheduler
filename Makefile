CC=clang
CFLAGS=-Wall -g -no-pie -O0 -Winline-asm -Wunused-variable

all: program

program: main.o SCHEDULER.o THREAD.o
	$(CC) $(CFLAGS) -o program main.o SCHEDULER.o THREAD.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

SCHEDULER.o: SCHEDULER.c
	$(CC) $(CFLAGS) -c SCHEDULER.c

THREAD.o: THREAD.c
	$(CC) $(CFLAGS) -c THREAD.c

clean:
	rm -f *.o program

run: program
	./program

