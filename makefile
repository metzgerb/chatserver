CC = gcc
CFLAGS = -g -Wall -std=c99

all : chatclient

chatclient : chatclient.c
	$(CC) $(CFLAGS) -o $@ $^

memCheck :
	valgrind --tool=memcheck --leak-check=yes chatclient

clean :
	-rm chatclient
