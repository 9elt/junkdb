CC=gcc
OPT=-O0
CFLAGS=-Wall -Wextra -g $(OPT)

all:
	$(CC) $(CFLAGS) src/server.c -o junkdb
	$(CC) $(CFLAGS) src/client.c -o junkdb-cli

clean:
	rm  junkdb junkdb-cli
