CC := gcc
CFLAGS := -O3 -Wall -std=c99
CLIB := -fopenmp -lm

all: bicgstab.o

bicgstab.o: bicgstab.c myblas.h SpUtil.h
	$(CC) $(CFLAGS) $< -o $@ $(CLIB)

