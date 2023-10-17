CC     = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -O2
CLIBS  = -lX11

SRC    = cgol.c \
	X11Window.c \
	utils.c \
	main.c

TARGET = cgol


all: build

build: $(SRC)
	$(CC) -g $(CFLAGS) $^ -o $(TARGET) $(CLIBS)

clean:
	rm -rf main.o X11Window.o cgol.o cgol

.PHONY: clean