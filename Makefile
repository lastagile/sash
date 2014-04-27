OBJS= wash.o buildin.o
CC=gcc
CFLAGS=-g

build:$(OBJS)
	$(CC) $(CFLAGS) -o wash $(OBJS)

clean:
	rm -rf *.o wash
