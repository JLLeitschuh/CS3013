CC=gcc
CFLAGS=-lpthread -std=c99
DEPS = Error.h Jobs.h SecurityLevel.h
OBJ = cluster.o Error.o Jobs.o

all: cluster

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cluster: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm cluster *.o
