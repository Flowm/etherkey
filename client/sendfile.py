#!/usr/bin/python

import socket
import time
import argparse

# Config
Remote_Host = '192.168.245.129'
Remote_Port =  5333
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.connect((Remote_Host, Remote_Port))


##########
def initConnection():
    global s
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((Remote_Host, Remote_Port))

def parseArguments():
    parser = argparse.ArgumentParser(description='USB-Network-Keyboard Client 0.01')
    parser.add_argument("-f", "--file", help="add a command file")
    parser.add_argument("-l", "--live", help="live interaction with Network-Keyboard")
    args = parser.parse_args()
    if args.file:
        print("Uploading command file to USB-Network-Keyboard ...")
        fileUpload(args.file)
        return

    elif args.live:
        print("Live-Mode started:")


def fileUpload(file):
    f = open(file)
    # TODO: Send command key for switching in XX Mode
    for line in iter(f):
        sendOnWire(line)
    f.close()
    print "Your file has been successfully sent"

def sendOnWire(msg):
    time.sleep(0.01)
    s.send(msg)

def main():
    initConnection()
    parseArguments()

if __name__=='__main__':
        main()
