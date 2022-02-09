UNAME_S = $(shell uname -s)

CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Ilib/cglm/include -Ilib/glad/include -Ilib/glfw/include -Ilib/stb -Ilib/noise -fbracket-depth=1024
LDFLAGS = lib/glad/src/glad.o lib/cglm/libcglm.a lib/glfw/src/libglfw3.a lib/noise/libnoise.a -lm

# GLFW required frameworks on OSX
ifeq ($(UNAME_S), Darwin)
	LDFLAGS += -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -ldl -lpthread
endif

SRC  = $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ  = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean libs game run dirs

game: $(BIN)/game

run: $(BIN)/game
	$<

libs: lib/cglm/libcglm.a lib/glad/src/glad.o lib/glfw/src/libglfw3.a lib/noise/libnoise.a

lib/cglm/libcglm.a:
	cd lib/cglm && cmake . -DCGLM_STATIC=ON && $(MAKE)

lib/glad/src/glad.o:
	cd lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c

lib/glfw/src/libglfw3.a:
	cd lib/glfw && cmake . && $(MAKE)

lib/noise/libnoise.a:
	cd lib/noise && $(MAKE)

dirs: $(BIN)

$(BIN):
	mkdir -p -- $@

$(BIN)/game: $(OBJ) | $(BIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.o: %.c | libs
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
