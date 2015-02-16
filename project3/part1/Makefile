CC=gcc
CFLAGS=-lpthread -std=gnu99
DEPS = Error.h Jobs.h Random.h ClusterManager.h SecurityLevel.h Boolean.h
OBJ = cluster.o Error.o Jobs.o Random.o ClusterManager.o

all: cluster

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cluster: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm cluster *.o
