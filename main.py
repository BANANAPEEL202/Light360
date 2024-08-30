import numpy as np
import pyautogui
from mss import mss
import cv2
from PIL import Image
from screen_splitter import *

screen_width, screen_height = pyautogui.size()
bounding_box = {
    'top': 0,
    'left': 0,
    'width': int(screen_width),
    'height': int(screen_height)
}

fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter("video.mp4", fourcc, 30, (bounding_box['width'], bounding_box['height']))

sct = mss()
frameNum = 0

while True:
    screenShot = sct.grab(bounding_box)
    img = Image.frombytes(
        'RGB', 
        (screenShot.width, screenShot.height), 
        screenShot.rgb, 
    )
    frame = np.array(img)
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)  # Convert RGB to BGR
    frame = cv2.resize(frame, (bounding_box['width'], bounding_box['height']))
    
    num_boxes = 100
    frame, boxes = split_frame_into_boxes(frame, num_boxes)
    out.write(frame)

    
out.release()
