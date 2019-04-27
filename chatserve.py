"""Program Name: chatserve.py
Python Version: 3
Description: Runs as a server to connect to chatclient and begin chat session
Author: Brian Metzger (metzgerb@oregonstate.edu)
Created: 2019-04-27
"""

import socket
import sys


def serve(port_number):
    print("cmd arg recd: %s" % port_number)



if __name__ == "__main__":
    #check total argument count
    if len(sys.argv) != 2:
        print("USAGE: %s port" % sys.argv[0])
    else:
        serve(sys.argv[1]);