CC=gcc
CFLAGS=-lpthread -lrt -std=gnu99
DEPS = Error.h Random.h Boolean.h Vehicles.h Intersection.h 
OBJ = intersection.o Random.c Error.c Vehicles.o Intersection.o

all: intersection

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

intersection: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm intersection *.o
