# Define (atleast) the following targets: all, clean
# all must build the executable file named mapreduce.
# clean must remove all object files and executable files created.
# Look at the introduction doc for information on how make a Makefile.
# This make file must build according to the structure described in the
# introduction doc, if it doesn't you will get a ZERO!

CFLAGS = -g -Wall -Werror -Iinclude

all: mapreduce

mapreduce: main.o map_reduce.o
	@mkdir -p bin
	gcc $(CFLAGS) build/main.o build/map_reduce.o -o bin/mapreduce

main.o: src/main.c
	@mkdir -p build 
	gcc $(CFLAGS) -c src/main.c -o build/main.o

map_reduce.o: src/map_reduce.c
	gcc $(CFLAGS) -c src/map_reduce.c -o build/map_reduce.o

clean:
	rm -rf bin build *.o mapreduce