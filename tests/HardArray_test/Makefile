CC=g++
CFLAGS=-c -Wall

all: HardArray_cluster

HardArray_cluster: HardArray_cluster.o
	$(CC) HardArray_cluster.o -o HardArray_cluster

HardArray_cluster.o: HardArray_cluster.cpp
	$(CC) $(CFLAGS) HardArray_cluster.cpp

clean:
	rm -rf *.o HardArray_cluster
