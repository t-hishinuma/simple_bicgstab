CC := gcc
CFLAGS := -O3 -fopenmp

all: bicgstab.o

bicgstab.o: bicgstab.c myblas.h
	$(CC) $(CFLAGS) $< -o $@

