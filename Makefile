CC = gcc

CFLAGS = -Wall -Wextra -std=c11
DEVFLAGS = -fsanitize=address -g
RELEASEFLAGS = -O2

SRC = src/main.c
OUT = build/app

dev:
	mkdir -p build
	$(CC) $(CFLAGS) $(DEVFLAGS) $(SRC) -o $(OUT)

build:
	mkdir -p build
	$(CC) $(CFLAGS) $(RELEASEFLAGS) $(SRC) -o $(OUT)

run: dev
	./$(OUT)

clean:
	rm -rf build
