CC=gcc
CFLAGS=-lpthread -std=gnu99
DEPS = Vehicles.h Boolean.h Error.h
OBJ = intersection.o Vehicles.o Error.c

all: instersection

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

instersection: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm instersection *.o
