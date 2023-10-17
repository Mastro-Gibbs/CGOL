CC     = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -O2
CLIBS  = -lX11

SRC    = src/cgol.c \
	src/X11Window.c \
	src/utils.c \
	src/main.c

TARGET = cgol


all: build

build: $(SRC)
	$(CC) -g $(CFLAGS) $^ -o $(TARGET) $(CLIBS)

clean:
	rm -rf main.o X11Window.o cgol.o cgol

.PHONY: clean