"""Program Name: chatserve.py
Python Version: 3
Description: Runs as a server to connect to chatclient and begin chat session
Author: Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-04-27
Last Modified: 2019-05-01
"""

from socket import *
import sys

#function that creates the server and sets it to listen for clients
def serve(port_number):
    #initialize values
    MAX_BUFFER = 700
    server_handle = "CHATSERVE> "
    
    #set up server socket
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', port_number))
    
    #continuously listen for a connection
    while True:
        server_socket.listen(1)
        
        #accept connection
        connection_socket, addr = server_socket.accept()
        
        #receive port number message from client
        #recd_message = connection_socket.recv(MAX_BUFFER).decode()
            
        #send message back
        #send_message = "%s Connection established." % server_handle
        #connection_socket.send(send_message.encode())
        
        #continuously receive messages
        while True:
            #receive message from client
            recd_message = connection_socket.recv(MAX_BUFFER).decode()
            
            #check if socket has closed using length of recd message
            if recd_message == "\\quit":
                break
            
            #print message to server
            print(recd_message)
            
            #get input from user
            message = input(server_handle)
            
            #check if message is "\quit"
            if message == "\\quit":
                #send quit message back
                send_message = "\\quit"
                connection_socket.send(send_message.encode())
                break
            
            #send message back
            send_message = server_handle + message
            connection_socket.send(send_message.encode())
        
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
        serve(int(sys.argv[1]));