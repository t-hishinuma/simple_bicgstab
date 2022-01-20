CC := gcc
CFLAGS := -O3 -Wall -std=c99
CLIB := -fopenmp -lm

LIB := include/myblas.h include/precond.h include/SpUtil.h
SRC := bicgstab.c

OBJ := bicgstab.o

.PHONY: test clean

all: $(OBJ)

$(OBJ): $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(CLIB)

test: $(OBJ)
	./$< ./data/test.mtx

clean:
	- rm $(OBJ)
