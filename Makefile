CC = gcc

CFLAGS = -Wall -Wextra -std=c11
DEVFLAGS = -fsanitize=address -g
RELEASEFLAGS = -O2

SRC = $(wildcard src/*.c)
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
    $(CC) $(CFLAGS) $(DEVFLAGS) $(SRC) $(TEST_SRC) -o $(TEST_OUT)
    ./$(TEST_OUT)

clean:
	rm -rf build
