CC = gcc
CFLAGS = -g -Wall -std=c99

all: chatclient chatserve

chatclient : chatclient.c
	$(CC) $(CFLAGS) -o $@ $^
	
cclient:
	rm -rf ./chatclient

chatserve: chatserve.py
	chmod +x $^

cserve: chatserve.py
	chmod -x $^

clean : cclient cserve
