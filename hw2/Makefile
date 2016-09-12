CC = gcc
SRC = $(wildcard src/*.c)
INCLUDE = $(wildcard include/)
BIN = utfconverter
CFLAGS = -g -Wall -Werror -pedantic -Wextra 
REQ = $(SRC) $(wildcard include/*.h)

all: $(BIN)

$(BIN): utfconverter.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -I $(INCLUDE) build/utfconverter.o -o bin/utfconverter

utfconverter.o: src/utfconverter.c
	@mkdir -p build
	$(CC) $(CFLAGS) -I $(INCLUDE) -c src/utfconverter.c -o build/utfconverter.o

.PHONY: clean

clean:
	rm -rf bin build *.o $(BIN)

