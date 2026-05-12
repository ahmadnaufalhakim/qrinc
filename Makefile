CC = gcc

CFLAGS = -Iinclude -Wall -Wextra -std=c11
DEVFLAGS = -fsanitize=address -g
RELEASEFLAGS = -O2

SRC = $(wildcard src/*.c)
APP_MAIN = src/main.c
LIB_SRC = $(filter-out $(APP_MAIN), $(SRC))
TEST_SRC = $(wildcard tests/*.c)

OUT = build/app
TEST_OUT = build/test

dev:
	mkdir -p build
	$(CC) $(CFLAGS) $(DEVFLAGS) $(SRC) -o $(OUT)

build:
	mkdir -p build
	$(CC) $(CFLAGS) $(RELEASEFLAGS) $(SRC) -o $(OUT)

run: dev
	./$(OUT)

test:
	mkdir -p build
	$(CC) $(CFLAGS) $(DEVFLAGS) $(LIB_SRC) $(TEST_SRC) -o $(TEST_OUT)
	./$(TEST_OUT)

clean:
	rm -rf build
