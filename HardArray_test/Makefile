CC=g++
CFLAGS=-c -Wall

all: arrayclusters

arrayclusters: arrayclusters.o
	$(CC) arrayclusters.o -o arrayclusters

arrayclusters.o: arrayclusters.cpp
	$(CC) $(CFLAGS) arrayclusters.cpp

clean:
	rm -rf *.o arrayclusters
