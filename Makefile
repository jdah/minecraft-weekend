UNAME_S = $(shell uname -s)

CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments
CFLAGS += -Isrc -Ilib/cglm/include -Ilib/glad/include -Ilib/glfw/include -Ilib/stb -Ilib/noise -fbracket-depth=1024

# Mitigaciones para compiladores modernos (GCC 16 / Clang)
ifeq ($(UNAME_S), Linux)
    CFLAGS += -D_GNU_SOURCE -Wno-error -Wno-array-parameter -Wno-stringop-overflow -Wno-array-bounds -Wno-deprecated-non-prototype
endif

LDFLAGS = lib/glad/src/glad.o lib/cglm/libcglm.a lib/glfw/src/libglfw3.a lib/noise/libnoise.a -lm

ifeq ($(UNAME_S), Darwin)
	LDFLAGS += -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -ldl -lpthread
endif

SRC  = $(sort $(shell find src -type f -name '*.c'))
OBJ  = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean run

all: dirs libs game

$(OBJ): libs

libs:
	cd lib/cglm && cmake . -DCGLM_STATIC=ON && $(MAKE)
	cd lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c
	cd lib/glfw && cmake . && $(MAKE)
	cd lib/noise && $(MAKE)

dirs:
	mkdir -p ./$(BIN)

run: all
	$(BIN)/game

game: dirs libs $(OBJ)
	$(CC) -o $(BIN)/game $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ)
	cd lib/cglm && $(MAKE) clean || true
	cd lib/glfw && $(MAKE) clean || true
	cd lib/noise && $(MAKE) clean || true
