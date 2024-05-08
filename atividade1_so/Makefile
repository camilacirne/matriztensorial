CC=gcc
CFLAGS=-Wall -Wextra -pthread

all: tensor

tensor: tensor.c
	$(CC) $(CFLAGS) -o tensor tensor.c

clean:
	rm -f tensor tensor.out
