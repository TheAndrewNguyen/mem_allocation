CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

OBJS = main.o memory.o

all: memory

memory: $(OBJS)
	$(CC) $(CFLAGS) -o memory $(OBJS)

main.o: main.c memory.h
	$(CC) $(CFLAGS) -c main.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

clean:
	rm -f memory $(OBJS)
