# Define (atleast) the following targets: all, clean
# all must build the executable file named mapreduce.
# clean must remove all object files and executable files created.
# Look at the introduction doc for information on how make a Makefile.
# This make file must build according to the structure described in the
# introduction doc, if it doesn't you will get a ZERO!

CFLAGS = -g -Wall -Werror -Iinclude

all: clean mapreduce

mapreduce: main.o map_reduce.o
	gcc -g -o mapreduce main.o map_reduce.o

#map_reduce:
#	gcc -Iinclude -Wall -Werror src/main.c

map_reduce.o:
	gcc $(CFLAGS) -c src/map_reduce.c

main.o:
	gcc $(CFLAGS) -c src/main.c

clean:
	rm -rf mapreduce
	rm -rf main.o
	rm -rf map_reduce.o

#all:
