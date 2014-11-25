#!/usr/bin/python

import argparse
import subprocess
from subprocess import Popen

# serial_cmd string
serial_cmd = 'cu -l /dev/ttyAMA0 -s 57600'

##########

def parseArguments():
    parser = argparse.ArgumentParser(description='USB-Net-Keyboard Client 0.01')
    parser.add_argument("-l", "--live", action='store_true', help="Live-Mode interaction with Net-Keyboard")
    parser.add_argument("-f", "--file", help="File-Mode send a command file")
    parser.add_argument("-s", "--server", help="login@server")
    args = parser.parse_args()

    if args.live and args.server:
        print("\nConnecting via Live-Mode:\n")
        ret = subprocess.call(["ssh", "-t", args.server, serial_cmd])

    elif args.file and args.server:
        print("\nConnecting via File-Mode:\n")

        copy = subprocess.Popen( ["cat - " + args.file + "| ssh " + args.server + " " + serial_cmd ], stdin=subprocess.PIPE, shell=True)
        copy.communicate("$'\cc'")

    else:
        print "Please use the following combinations\n"
        print "LIVE-Mode: ./EtherkeyClient -l -s login@server"
        print "FILE-Mode: ./EtherkeyClient -f FILE -s login@server\n"
        return

def main():
    parseArguments()

if __name__=='__main__':
        main()
