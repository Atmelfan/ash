#!/home/ash/.virtualenvs/cv3/bin/python
import serial

port = serial.Serial("/dev/ttyAMA0", baudrate=115200, timeout=3.0)

while True:
    port.write("\r\nSay something:")
    rcv = port.read(10)
    port.write("\r\nYou sent:" + repr(rcv))

