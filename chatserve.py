"""Program Name: chatserve.py
Python Version: 3
Description: Runs as a server to connect to chatclient and begin chat session
Author: Brian Metzger (metzgerb@oregonstate.edu)
Course: CS372 (Spring 2019)
Created: 2019-04-27
Last Modified: 2019-05-01
"""

from socket import *
import sys

#funtion to send a message to a specified client
def send_msg(client_socket, handle, sentinel):
    #get input from user
    message = input(handle)
            
    #check if message is "\quit"
    if message == "\\quit":
        #send quit message back with sentinel
        message += sentinel
        connection_socket.sendall(message.encode())
        return -1          
    else: 
        #send message back
        message = server_handle + message + sentinel
        client_socket.sendall(message.encode())
        return 0

#function to receive a message from a specified client
def recv_msg(client_socket, sentinel, max_buffer):
    #set initial values to empty strings
    buffer = ""
    message = ""
    
    #call recv in a loop until sentinel is detected in compiled message
    while(SENTINEL not in message):
        buffer = client_socket.recv(max_buffer).decode()
        message += buffer
        
    #remove sentinel from message
    message = message.replace(sentinel, "")
    
    #check if socket has closed using length of recd message
    if message == "\\quit":
        return -1
    else:    
        #print message to server
        print(message)
        return message

#function that creates the server and sets it to listen for clients
def setup_server(port_number):
    #initialize values
    MAX_BUFFER = 700
    SENTINEL = "@!@"
    server_handle = "CHATSERVE> "
    
    #set up server socket
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', port_number))
    
    #continuously listen for a connection
    while True:
        server_socket.listen(1)
        
        #accept connection
        connection_socket, addr = server_socket.accept()

        #continuously receive and send messages
        while True:
            #receive a message from client
            recd_message = recv_msg(connection_socket, SENTINEL, 700)
            
            #check if received message was quit
            if recd_message == -1:
                break
            
            #send message to client
            send_message = send_msg(connection_socket, server_handle, SENTINEL)
            
            #check if sent message was quit
            if send_message == -1:
                break
        
        #close connection
        connection_socket.close()
        

#determine program execution
if __name__ == "__main__":
    #check total argument count
    if len(sys.argv) != 2:
        print("USAGE: %s port" % sys.argv[0])
    elif not sys.argv[1].isdigit():
        print("syntax error: port must be a number")
    else:
        setup_server(int(sys.argv[1]));