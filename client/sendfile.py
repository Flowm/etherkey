#!/usr/bin/python

import socket
import time

# Config
Remote_Host = '192.168.245.129'
Remote_Port =  5333
Localfile = 'text.txt'

##########
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((Remote_Host, Remote_Port))

def sendOnWire(msg):
    time.sleep(0.01)
    s.send(msg)

def readFile():
    f = open('test.txt')
    for line in iter(f):
        sendOnWire(line)
    f.close()

def main():
    readFile()

if __name__=='__main__':
        main()
