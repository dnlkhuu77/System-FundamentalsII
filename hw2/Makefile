CC = gcc
SRC = $(wildcard src/*.c)
INCLUDE =  include
BIN = utfconverter
CFLAGS = -g -Wall -Werror -Iinclude -pedantic -Wextra -DDEBUG
REQ = $(SRC) $(wildcard include/*.h)

all: clean utf

utf: utf_convert.o
	@mkdir -p bin
	$(CC) $(CFLAGS) build/utf_convert.o -o bin/utf

utf_convert.o: src/utfconverter.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/utfconverter.c -o build/utf_convert.o

debug: utf_convert.o
	@mkdir -p bin
	$(CC) $(CFLAGS) build/utf_convert.o -o bin/utf

clean:
	rm -rf bin build *.o utf
