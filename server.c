#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 500
#define MAX_BUFFER 700

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	
	//listen indefinitely
	while (1)
	{
		listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		// Send a Success message back to the client
		charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
		if (charsRead < 0) error("ERROR writing to socket");

		//loop until '\quit' received from user
		while(1)
		{
			// Get the message from the client and display it
			memset(message, '\0', sizeof(message));
			charsRead = recv(establishedConnectionFD, message, sizeof(message) - 1, 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");
			printf("%s\n", message);

			// Get input message from user
			printf("CHATSERVE> ");
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
			strcpy(message, "CHATSERVE> ");
			strcat(message, buffer);

			// Send message to server
			charsWritten = send(establishedConnectionFD, message, strlen(message), 0); // Write to the server
			if (charsWritten < 0) error("ERROR writing to socket");
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}

	close(listenSocketFD); // Close the listening socket
	return 0; 
}
