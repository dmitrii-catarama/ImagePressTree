.PHONY: build clean

CC = gcc
CFLAGS = -Wall -Wextra -Werror

build: quadtree

quadtree: quadtree.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

quadtree.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f quadtree quadtree.o
