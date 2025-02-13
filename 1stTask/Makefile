CC=g++
CFLAGS=-std=c++11 -Wall -Wextra

all: float double

float:
    $(CC) $(CFLAGS) -DUSE_DOUBLE=0 main.cpp -o float

double:
    $(CC) $(CFLAGS) -DUSE_DOUBLE=1 main.cpp -o double

clean:
    rm -f float double