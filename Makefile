CC     = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -O2
CLIBS  = -lX11

SRC_DIR = src
OBJ_DIR = object

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

TARGET = cgol

all: build

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CLIBS)

build: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(CLIBS)

clean:
	rm -rf $(OBJECTS) cgol

.PHONY: clean
