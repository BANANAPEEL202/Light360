from SerialCommunicator import SerialCommunicator
from spotify_module import *
import time
  
serial = SerialCommunicator(port='/dev/cu.usbmodem11101')
time.sleep(1)
print("Connected to Arduino")


#serial.set_mode(1)
serial.send("colors")

#serial.send_colors([(255, 0, 0), (0, 255, 0)])


#serial.send_colors(get_spotify_colors(5))