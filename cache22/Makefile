flags=-O2 -Wall -std=c2x
ldflags=

all:clean cache22
cache22:cache22.o
	cc $(flags) $^ -o $@  $(ldflags)

cache22.o:cache22.c cache22.h
	cc $(flags) -c $^

clean:
	rm -rf *.o cache22