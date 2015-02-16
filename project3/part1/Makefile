CC=gcc
CFLAGS=-lpthread -std=gnu99
DEPS = Error.h Jobs.h SecurityLevel.h Random.h
OBJ = cluster.o Error.o Jobs.o Random.o

all: cluster

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cluster: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm cluster *.o
