CC = gcc
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt

all: proj2
proj2: proj2.c proj2.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o proj2 testing