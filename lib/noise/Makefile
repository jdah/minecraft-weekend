CC      = gcc
CFLAGS  = -std=c11 -O3
LDFLAGS =

.PHONY: all clean

all: lib

%.o: %.c
	$(CC) -o $@ $^ -c $(CCFLAGS) $(LDFLAGS)

lib: noise1234.o
	ar rcs libnoise.a $^

clean:
	rm -rf libnoise.a