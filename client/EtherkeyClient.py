#!/usr/bin/env python2

import socket
import time
import argparse
import subprocess

# File-Mode Configuration
rHost = '192.168.245.129'
rPort =  1234

# Live-Mode Configuration
login = 'root'
serial_cmd = 'cu -l /dev/ttyAMA0 -s 57600'

##########

def initConnection():
    global s
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((rHost, rPort))

def parseArguments():
    parser = argparse.ArgumentParser(description='USB-Net-Keyboard Client 0.01')
    parser.add_argument("-f", "--file", help="add a command file")
    parser.add_argument("-l", "--live", help="live interaction with Net-Keyboard")
    args = parser.parse_args()
    if args.file:
        print("Uploading command file to USB-Network-Keyboard ...")
        fileUpload(args.file)
        return

    elif args.live:
        print("Live-Mode started:")
        ret = subprocess.call(["ssh", "-t", login + "@" + rHost, serial_cmd])

    else:
        print "No argument submitted"
        return

def fileUpload(file):
    f = open(file)

    # Send CTRL+Q and 1 for switching into Mode 1
    s.send(chr(17))
    s.send('1')

    for line in iter(f):
        s.send(line)
        time.sleep(0.01)
    f.close()
    print "Your file has been successfully sent"

def main():
    initConnection()
    parseArguments()

if __name__=='__main__':
        main()
