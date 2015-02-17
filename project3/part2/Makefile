CC=gcc
CFLAGS=-lpthread -std=gnu99
DEPS = car.h Boolean.h Error.h
OBJ = intersect.o Error.c

all: intersect

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

instersection: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm intersect *.o
