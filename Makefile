CC = gcc

CFLAGS = \
	-Iinclude \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wsign-conversion \
	-Wno-override-init \
	-std=c11
DEVFLAGS = \
	-fsanitize=address,undefined \
	-fno-omit-frame-pointer \
	-g3 \
	-fanalyzer
RELEASEFLAGS = -O2

SRC = $(wildcard src/*.c)
APP_MAIN = src/main.c
LIB_SRC = $(filter-out $(APP_MAIN), $(SRC))

TEST_MAIN = tests/test_main.c
TEST_SRC = $(filter-out $(TEST_MAIN), $(wildcard tests/*.c))

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
	$(CC) $(CFLAGS) $(DEVFLAGS) $(LIB_SRC) $(TEST_SRC) $(TEST_MAIN) -o $(TEST_OUT)
	./$(TEST_OUT)

clean:
	rm -rf build
