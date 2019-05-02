CS372 Project 1 README.txt
Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-05-01

INSTRUCTIONS:
1. compile chatclient using the provided makefile
		(the command in the makefile is: "gcc -g -Wall -std=c99 -o chatclient chatclient.c")
2. run chatserve.py using command: python3 chatserve.py [portnum]
		(portnum is the port you wish to run the server on)
3. run chatclient using command: ./chatclient [host] [portnum]
		(I used flip3.engr.oregonstate.edu 23456 for my host and port number during testing)
4. chatclient will prompt user for a handle and then allow the user to input a message to send
5. chatserve will receive the message and allow the server user to respond with their on message
6. chatclient will receive the response and allow the user to input a message again
	(steps 5-6 repeat until on of the users enters the message "\quit")
	
To QUIT:
1. chatclient or chatserve must send the message "\quit" to close the connection
2. chatserve will continue running to accept a new connection
3. chatclient will close and return to the terminal prompt

NOTE:
"make clean" will remove the chatclient executable from the directory