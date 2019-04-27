CC = gcc
CFLAGS = -g -Wall -std=c99

chatclient : chatclient.c
	$(CC) $(CFLAGS) -o $@ $^
	
server : server.c
	$(CC) $(CFLAGS) -o $@ $^

clean :
	-rm chatclient
