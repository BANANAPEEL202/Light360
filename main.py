from SerialCommunicator import SerialCommunicator
from spotify_module import *
import time
  
serial = SerialCommunicator(port='/dev/cu.usbmodem11101')
time.sleep(1)
print("Connected to Arduino")


#serial.set_mode(1)


print(get_spotify_colors(2))
#serial.send_colors(get_spotify_colors(2))
serial.send_colors([(0, 250, 0)])
