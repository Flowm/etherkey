#!/usr/bin/python
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018 Sony Corporation
#
# Author: Tim Bird <tim.bird@sony.com>
#
# This assumes that the teensy has the etherkey sketch loaded,
# and that the serial port (pins gnd,0,1) are connected to the
# local machine via some device (e.g. USB serial, in the examples
# below)
#

import serial
import sys
import time

def usage(rcode):
    print """Usage: teensy-usb.py [options] <device> <commands>

Sends the keystrokes indicated by <commands> to the
machine connected via USB to the teensy board.

-h         = show this usage help
-c         = show a command string cheatsheet
<device>   = e.g. /dev/ttyUSB1
<commands> = words or letters use to produce keystrokes for the "Command Mode"
             of teensy running the etherkey sketch.
             (see  https://github.com/Flowm/etherkey)

The <commands> are same as etherkey command mode strings, with
one addition.  You can also specify sleep=x, where x is a floating
point number (e.g. 0.2)

Example:
Here is a sequence that will send 2 down arrows, separated by a half second:
 $ teensy-usb.py /dev/ttyUSB0 {down} sleep=0.5 {down}

By default, teensy-usb.py will put a .1 second delay between each command
transmitted.
"""
    exit(rcode)

def cheatsheet():
    print """        Keystroke cheat sheet
        -------------------------
Regular characters: send those character as keystrokes
ex: "Now is the time for all good men..."

Modifiers: ! = send next char with ALT key pressed
+ = SHIFT, ^ = CTRL, # = WIN
ex: ab^c  - sends 'a' and 'b', and CTRL-C

Named keys: {enter}, {escape}, {space}, {tab}, {backspace}/{BS},
{delete}/{del}, {insert}/{ins}, {up}, {down}, {left}, {right},
{home}, {end}, {pgup}, {pgdn}

Escaped characters:  To send a single character literally, use braces:
{x}, {!}, {+}, {^} {#}, {{} {}}

Repeats: put number of iterations inside the brace, after the item:
ex: {x 10} - sends 'x' 10 times, and {Enter 5} sends 5 Enters
"""
    exit(0)


if "-h" in sys.argv or "--help" in sys.argv:
    usage(0)

if "-c" in sys.argv or "--cheatsheet" in sys.argv:
    cheatsheet()

if len(sys.argv)<3:
    print("Not enough command line arguments")
    usage(1)


device = sys.argv[1]
commands = sys.argv[2:]
ser = serial.Serial(device, 57600)

# make sure we're in Command mode
# send ctrl-Q, then 1
ser.write("\x11")
time.sleep(0.1)
ser.write("1")
time.sleep(0.1)

# now send commands
for cmd in commands:
    if cmd.startswith("sleep="):
        amount=float(cmd[6:])
        time.sleep(amount)
        continue

    ser.write("Send %s\n" % cmd)
    time.sleep(0.1)

ser.close()
