__author__ = "MuT6 Sch01aR"

 
from PIL import Image,ImageFilter,ImageGrab,ImageDraw,ImageFont
import socket
import sys
import os
import struct
import threading
import time

isSWAP = True # 是否高低位交换
server = socket.socket()
server.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 0)
server.bind(('192.168.0.104',8080))
server.listen()

imgW = 160
imgH = 80

count = 0

def countCalc():
    print(count)

def png2rgb565(im):

    binoutfile = open("binData.bin", "wb")
    bytesList = b''
    swapLast = ''
    pix = im.load()  #load pixel array
    for h in range(imgH):
        for w in range(imgW):
            if w < im.size[0]:
                R=pix[w,h][0]>>3
                G=pix[w,h][1]>>2
                B=pix[w,h][2]>>3
                rgb = (R<<11) | (G<<5) | B
                if (isSWAP == True):
                    swap_string_low = rgb >> 8
                    swap_string_high = (rgb & 0x00FF) << 8
                    swap_string = swap_string_low | swap_string_high
                    binoutfile.write(struct.pack('H', swap_string))
                else:
                    print(".")
            else:
                rgb = 0
    binoutfile.close()

def catchScreen():
    
    imGrab = ImageGrab.grab()
    imGrab = imGrab.resize((imgW, imgH))
    png2rgb565(imGrab)

def sendBinData():
    
    conn,addr = server.accept()
    while True:
        catchScreen()
        binFile = open("binData.bin", "rb")
        binData = binFile.read()
        conn.send(binData)
        time.sleep(0.05)
        
    server.close()
# 帧率测试代码
# flag = 1
# if(flag % 2 == 0):
#     conn.send(binData)
# else:
#     conn.send(binData1)
# global count
# timer = threading.Timer(1, countCalc)
# timer.start()
# count += 1
# flag += 1
# binFile1 = open("grab.bin", "rb")
# binData1 = binFile1.read()

sendBinData()
