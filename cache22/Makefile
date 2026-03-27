flags=-O2 -Wall -std=c2x
ldflags=

all:clean cache22 tree
tree: tree.o
	cc $(flags) $^ -o $@ $(ldflags)
tree.o: tree.c tree.h
	cc $(flags) -c $^ 

cache22:cache22.o
	cc $(flags) $^ -o $@  $(ldflags)

cache22.o:cache22.c cache22.h
	cc $(flags) -c $^

clean:
	rm -rf *.o cache22 tree 