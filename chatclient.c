/******************************************************************************
* Program Name: chatclient.c
* Description: Runs as a client to connect to chatserve and begin chat session
* Sources: Base code used from CS344 client.c file (winter 2019 term) and as
		noted within code.
* Author: Brian Metzger (metzgerb@oregonstate.edu)
* Course: CS372 (Spring 2019)
* Created: 2019-04-26
* Last Modified: 2019-05-02
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFFER_SIZE 503
#define MAX_BUFFER 700
#define SENTINEL "@!@"

//function declarations
void error(const char *msg);
int connectServer(char* server, int portNumber);
int sendMsg(int socketPtr, char handle[]);
int recvMsg(int socketPtr);


int main(int argc, char *argv[])
{
	int socketFD, msgResult;
	char handle[11];
    
	//check for correct number of arguments
	if (argc < 3) 
	{ 
		fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
		exit(0); 
	}

	//create socket and connect to server
	socketFD = connectServer(argv[1], atoi(argv[2]));

	//check for socket/connection errors
	if (socketFD == -1)
	{
		error("# CLIENT: ERROR opening socket");
	}
	else if (socketFD == -2)
	{
		error("# CLIENT: ERROR connecting");
	}

	//get handle from user
	printf("Enter a handle: ");
	fflush(stdout);
	memset(handle, '\0', sizeof(handle)); // Clear out the buffer array
	fgets(handle, sizeof(handle) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	handle[strcspn(handle, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	//loop while sending and receiving messages
	while (1)
	{
		//receive a message from the server
		msgResult = sendMsg(socketFD, handle);

		//check for a quit command indicator
		if (msgResult == 1)
		{
			//move to close the connection
			break;
		}

		//receive a message from the server
		msgResult = recvMsg(socketFD);
		
		//check for a quit command indicator
		if (msgResult == 1)
		{
			//move to close the connection
			break;
		}
	}

	close(socketFD); // Close the socket
	return 0;
}


/******************************************************************************
 * Function name: error
 * Inputs: Takes a message
 * Outputs: Returns nothing
 * Description: The function prints the error and exits the program.
 ******************************************************************************/
void error(const char *msg) 
{ 
	perror(msg); 
	exit(0); 
}


/******************************************************************************
 * Function name: connectServer
 * Inputs: Takes the server and port number of the server
 * Outputs: Returns a socket if connected successfully
 * Description: The function attempts to connect to the specified server and 
		port using a TCP connection. If socket fails to open, function return -1
		if socket fails to connect to server, function returns -2.
 ******************************************************************************/
int connectServer(char* server, int portNumber)
{
	int socketPtr;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(server); // Convert the machine name into a special form of address
	
	//check if server info could not be obtained
	if (serverHostInfo == NULL) 
	{ 
		error("# CLIENT: ERROR, no such host\n"); 
	}

	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr_list[0], serverHostInfo->h_length); 

	// Set up the socket
	socketPtr = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	//check that socket was opened successfully
	if (socketPtr < 0)
	{
		return -1;
	}

	// Connect to server and check that connection was successful
	if (connect(socketPtr, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		return -2;
	}
	
	return socketPtr;
}


/******************************************************************************
 * Function name: sendMsg
 * Inputs: Takes the socket to send to and a handle to prepend to messages
 * Outputs: Returns an integer
 * Description: The function gets a message from the user. If a quit
 *		command is received, the function sends a quit command to the server and
 *		returns 1. All other messages are sent to the server and 0 is returned.
 ******************************************************************************/
int sendMsg(int socketPtr, char handle[])
{
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	int charsWritten;
	
	// Get input message from user
	printf("%s> ", handle);
	fflush(stdout);
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	//check if '\quit' command received
	if (strcmp(buffer, "\\quit") == 0)
	{
		//Add sentinel before sending quit to server
		strcat(buffer, SENTINEL);
		//stop client, send  move to close connection
		charsWritten = send(socketPtr, buffer, strlen(buffer), 0); // Write to the server
		
		//check for errors
		if (charsWritten < 0)
		{
			error("# CLIENT: ERROR writing to socket");
		}
		if (charsWritten < strlen(buffer)) //I chose to not loop while sending since the message is small. This will catch any error caused by that choice
		{
			printf("# CLIENT: WARNING: Not all data written to socket!\n");
		}

		return 1;
	}

	//prepend handle to message
	memset(message, '\0', sizeof(message)); // Clear out the message array
	strcpy(message, handle);
	strcat(message, "> ");
	strcat(message, buffer);
	strcat(message, SENTINEL);

	// Send message to server
	long length = strlen(message) + 1;
	char* sendPtr = message;

	//loop and send message until all is sent
	//modified from source: https://stackoverflow.com/questions/13479760/c-socket-recv-and-send-all-data
	while (length > 0)
	{
		long s = send(socketPtr, sendPtr, length, 0);

		//check for write error
		if (s < 0)
		{
			error("# CLIENT: ERROR writing to socket");
		}
		sendPtr += s;
		length -= s;
	}

	return 0;
}


/******************************************************************************
 * Function name: recvMsg
 * Inputs: Takes the socket to receive from
 * Outputs: Returns an integer
 * Description: The function receives a message from the server. If a quit 
 *		command is received, the function returns 1. All other messages are 
 *		printed and 0 is returned
 ******************************************************************************/
int recvMsg(int socketPtr)
{
	char buffer[BUFFER_SIZE];
	char message[MAX_BUFFER];
	int charsRead;

	memset(buffer, '\0', sizeof(buffer));
	memset(message, '\0', sizeof(message));

	// Get return message from server
	while (strstr(buffer, SENTINEL) == NULL)
	{
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketPtr, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		
		//check for socket read error
		if (charsRead < 0)
		{
			error("# CLIENT: ERROR reading from socket");
		}

		//add new characters
		strcat(message, buffer);
	}

	//strip term sentinel from return message
	message[strlen(message) - strlen(SENTINEL)] = '\0';

	//check for quit message from server
	if (strcmp(message, "\\quit") == 0)
	{
		//return quit command
		return 1;
	}

	printf("%s\n", message);
	fflush(stdout);

	return 0;
}