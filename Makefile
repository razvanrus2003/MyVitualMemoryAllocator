# Copyright 2022 Echipa PCLP1

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

# define targets
TARGETS = vma

run_vma: vma
	./vma

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

vma.o: vma.c vma.h
	$(CC) $(CFLAGS) -c vma.c

gen_func.o: gen_func.c gen_func.h
	$(CC) $(CFLAGS) -c gen_func.c

aux_func.o: aux_func.c aux_func.h
	$(CC) $(CFLAGS) -c aux_func.c

dll.o: dll.c dll.h
	$(CC) $(CFLAGS) -c dll.c

build: main.o vma.o gen_func.o aux_func.o dll.o
	$(CC) $(CFLAGS) main.o vma.o gen_func.o aux_func.o dll.o  -lm -o vma

pack:
	zip -FSr 312CA_RusRazvan_Tema1.zip Makefile *.c *.h README

clean:
	rm -f $(TARGETS) *o

.PHONY: pack clean
