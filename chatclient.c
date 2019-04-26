/******************************************************************************
* Program Name: chatclient.c
* Description: Runs as a client to connect to chatserve and begin chat session
* Author: Brian Metzger (metzgerb@oregonstate.edu)
* Created: 2019-04-26
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

#define _POSIX_C_SOURCE 200112L
#define MAX_BUFFER 700
#define TERM_SENTINEL "!$@"

//error function used for reporting errors
void error(const char *msg) 
{ 
	perror(msg); 
	fflush(stderr);
	exit(0);
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	char buffer[MAX_BUFFER];
	char message[2 * MAX_BUFFER];

	// Check usage & args
	if (argc != 3) 
	{
		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
		fflush(stderr);
		exit(0); 
	}
	
	// Set up the server address struct
	/*memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	*/

	//set up server address for connection
	//Source: beej's guide to network programming
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	// servinfo now points to a linked list of 1 or more struct addrinfos

	// ... do everything until you don't need servinfo anymore ....

	//freeaddrinfo(servinfo); // free the linked-list MOVE TO BOTTOM WHEN FINISHED


	// Set up the socket
	socketFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol); // Create the socket
	if (socketFD < 0)
	{
		 error("CLIENT: ERROR opening socket");
	}

	// Connect to server
	if (connect(socketFD, servinfo->ai_addr, servinfo->ai_addrlen) < 0) // Connect socket to address
	{	
		error("CLIENT: ERROR connecting");
	}

	//TODO: Send client port number 
	char* msgType = "e";

	// Send client port number to server
	charsWritten = send(socketFD, msgType, 2, 0); // Write to the server
	
	//check for socket write error
	if (charsWritten < 0)
	{
		 error("CLIENT: ERROR writing to socket");
	}

	//check for all data written
	if (charsWritten < 2)
	{
		 printf("CLIENT: WARNING: Not all data written to socket!\n");
		fflush(stdout);
	}

	// Get return message from server (only expecting a single char)
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, 2, 0); // Read data from the socket, leaving \0 at end
	
	//check for socket read error
	if (charsRead < 0) 
	{
		error("CLIENT: ERROR reading from socket");
	}

	//create message
	memset(message, '\0', sizeof(message));
	strcat(message, TERM_SENTINEL);
	
	// Send message to server
	long length = strlen(message) + 1;
	char* sendPtr = message;
	
	//loop and send message until all is sent
	//modified from source: https://stackoverflow.com/questions/13479760/c-socket-recv-and-send-all-data
	while (length > 0)
	{
		long s = send(socketFD, sendPtr, length, 0);
		
		//check for write error
		if (s < 0)
		{
			error("CLIENT: ERROR writing to socket");
		}
		sendPtr += s;
		length -= s;
	}
	
	// Get return message from server
	memset(message, '\0', sizeof(message));
	
	while(strstr(buffer, TERM_SENTINEL) == NULL)
	{
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		//check for socket read error
		if (charsRead < 0) 
		{
			error("CLIENT: ERROR reading from socket");
		}
	
		//add new characters
		strcat(message, buffer);
	}

	//strip term sentinel from return message
	message[strlen(message) - 1] = '\0';
	printf("%s\n", message);
	fflush(stdout);

	close(socketFD); // Close the socket

	return 0;
}
