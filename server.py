import socket
import sys

import cv2
import numpy as np

import struct

HOST = None               # Symbolic name meaning all available interfaces
PORT = 7200              # Arbitrary non-privileged port
s = None
FRAME_WIDTH = 640
FRAME_HEIGHT = 480
CHANNEL=3

for res in socket.getaddrinfo(HOST, PORT, socket.AF_UNSPEC,
                              socket.SOCK_STREAM, 0, socket.AI_PASSIVE):
    af, socktype, proto, canonname, sa = res
    try:
        s = socket.socket(af, socktype, proto)
    except socket.error as msg:
        s = None
        continue
    try:
        s.bind(sa)
        s.listen(1)
    except socket.error as msg:
        s.close()
        s = None
        continue
    break
if s is None:
    print 'could not open socket'
    sys.exit(1)
conn, addr = s.accept()
print 'Connected by', addr

running = True
while running:
    i, ptr = (0,0)
    shape = (FRAME_HEIGHT, FRAME_WIDTH, CHANNEL)
    cameraFeed = np.zeros(shape, np.uint8)
    imgSize = cameraFeed.size
    sockData = b''

    while imgSize:
        nbytes=conn.recv(imgSize)
        if not nbytes: break
        sockData+=nbytes
        imgSize-=len(nbytes)

    unp = lambda leng: struct.unpack('B', sockData[ptr+leng])[0]

    for i in range(0, cameraFeed.shape[0]):
        for j in range(0, cameraFeed.shape[1]):
            cameraFeed[i,j] = [unp(0),unp(1),unp(2)]
            ptr=ptr+3

    # Create a window for display.
    cv2.namedWindow("server");
    cv2.imshow("server", cameraFeed)
    key = cv2.waitKey(30)
    running = key

    # esc
    if key==27:
        running =False

    # data = conn.recv(1024)
    # if not data: break
    # conn.send(data)
conn.close()
