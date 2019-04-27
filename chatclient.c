/******************************************************************************
* Program Name: chatclient.c
* Description: Runs as a client to connect to chatserve and begin chat session
* Sources: Base code used from CS344 client.c file (winter 2019 term)
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
#include <netdb.h> 

#define BUFFER_SIZE 500
#define MAX_BUFFER 700

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	char handle[11];
    
	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "# CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr_list[0], serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("# CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("# CLIENT: ERROR connecting");

	//get handle from user
	printf("Enter a handle: ");
	memset(handle, '\0', sizeof(handle)); // Clear out the buffer array
	fgets(handle, sizeof(handle) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	handle[strcspn(handle, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	//prompt user to send port number
	printf("Client port number: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	//send initial message
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("# CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("# CLIENT: WARNING: Not all data written to socket!\n");

	//wait for server response before beginning normal chat process
	memset(message, '\0', sizeof(message)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, message, sizeof(message) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("# CLIENT: ERROR reading from socket");
	printf("%s\n", message);

	while (1)
	{
		// Get input message from user
		printf("%s> ", handle);
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
		fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
		buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

		//check if '\quit' command received
		if (strcmp(buffer, "\\quit") == 0)
		{
			//stop client and move to close connection
			break;
		}

		//prepend handle to message
		memset(message, '\0', sizeof(message)); // Clear out the message array
		strcpy(message, handle);
		strcat(message, "> ");
		strcat(message, buffer);

		// Send message to server
		charsWritten = send(socketFD, message, strlen(message), 0); // Write to the server
		if (charsWritten < 0) error("# CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(message)) printf("# CLIENT: WARNING: Not all data written to socket!\n");

		// Get return message from server
		memset(message, '\0', sizeof(message)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, message, sizeof(message) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("# CLIENT: ERROR reading from socket");
		printf("%s\n", message);
	}

	close(socketFD); // Close the socket
	return 0;
}