"""Program Name: chatserve.py
Python Version: 3
Description: Runs as a server to connect to chatclient and begin chat session
Author: Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-04-27
"""

import socket
import sys

#function that creates the server and sets it to listen for clients
def serve(port_number):
    #initialize message length
    MAX_BUFFER = 700
    
    #set up server socket
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', port_number))
    
    #continuously listen for a connection
    while True:
        server_socket.listen(1)
        
        #accept connection
        connection_socket, addr = server_socket.accept()
        
        #continuously receive messages
        while True:
            #receive message from client
            recd_message = connection_socket.recv(MAX_BUFFER).decode()
            
            #send message back
            send_message = "CHATSERVE> Received message: %s" % recd_message
            connection_socket.send(send_message.encode())
        
        #close connection
        connection_socket.close()
        

#determine program execution
if __name__ == "__main__":
    #check total argument count
    if len(sys.argv) != 2:
        print("USAGE: %s port" % sys.argv[0])
    else:
        serve(sys.argv[1]);