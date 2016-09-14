CC = gcc
SRC = $(wildcard src/*.c)
INCLUDE =  include
BIN = utfconverter
CFLAGS = -g -Wall -Werror -Iinclude -pedantic -Wextra 
REQ = $(SRC) $(wildcard include/*.h)

all: utfconverter

utfconverter: utfconverter.o
	@mkdir -p bin
	$(CC) $(CFLAGS) build/utfconverter.o -o bin/utfconverter

utfconverter.o: src/utfconverter.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/utfconverter.c -o build/utfconverter.o

debug: utfconverter.o
	@mkdir -p bin
	$(CC) $(CFLAGS) build/utfconverter.o -o bin/utfconverter 

.PHONY: clean

clean:
	rm -rf bin build *.o utfconverter
