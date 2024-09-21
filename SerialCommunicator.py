# Importing Libraries 
import serial 
from serial.tools import list_ports
import time

class SerialCommunicator:
    def __init__(self, port, baudrate=115200, timeout=0.1):
        self.arduino = serial.Serial(port=port, baudrate=baudrate, timeout=timeout) 

    def set_mode(self, mode): 
        #1 = Spotify Ambient Mode
        msg = 'mode: ' + str(mode)
        self.send(msg)

    def send_colors(self, colors):
        num_colors = len(colors)
        msg = 'colors: ' + str(num_colors)
        for color in colors:
            msg += '|' + str(color[0]) + ',' + str(color[1]) + ',' + str(color[2])
        self.send(msg)

    def send(self, msg):
        #print(msg)
        msg = "<" + msg + ">"
        #msg = bytes(msg, 'utf-8')
        for i in msg:
            self.arduino.write(i.encode())
            time.sleep(0.1)
        #self.arduino.write(b'<color>')

    def read(self): 
        data = self.arduino.readline() 
        return data


