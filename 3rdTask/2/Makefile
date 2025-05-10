CC = g++
CFLAGS = -std=c++20 -pthread

all: program

program: main.o
	$(CC) $(CFLAGS) main.o -o program

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o program