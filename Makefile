CC := gcc
CFLAGS := -O3 -Wall -std=c99
CLIB := -fopenmp -lm

.PHONY: test

all: bicgstab.o

bicgstab.o: bicgstab.c myblas.h SpUtil.h
	$(CC) $(CFLAGS) $< -o $@ $(CLIB)

test: bicgstab.o
	$@ ./data/test.mtx
