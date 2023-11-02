CC=g++
CFLAGS=-c -Wall

all: UserInput_cluster

UserInput_cluster: UserInput_cluster.o
	$(CC) UserInput_cluster.o -o UserInput_cluster

UserInput_cluster.o: UserInput_cluster.cpp
	$(CC) $(CFLAGS) UserInput_cluster.cpp

clean:
	rm -rf *.o UserInput_cluster
