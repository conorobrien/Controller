#! /usr/bin/env python

import serial;
import numpy as np;
import pylab as plt
import time
import thread

def input_thread(list):
    raw_input()
    list.append(None)

plt.ion()
fig, ax = plt.subplots()
data1 = np.zeros(1000)
data2 = np.zeros(1000)
live_plot1, = ax.plot(data1)
live_plot2, = ax.plot(data2)
ax.plot([0, 1000], [125, 125])
plt.ylim([-10,265])
ax.grid(True)

flag = []
thread.start_new_thread(input_thread, (flag,))

ser = serial.Serial("/dev/tty.usbmodem411")

ser.write(' ')

t0 = time.time()
i = 1;
while True:
    i += 1
    data1 = np.roll(data1, -1)
    data2 = np.roll(data2, -1)
    data1[-1] = ord(ser.read(1))
    data2[-1] = ord(ser.read(1))
    if i%5 == 0:
        live_plot1.set_ydata(data1)
        live_plot2.set_ydata(data2)
        plt.pause(0.005)
    if flag:
        break

print (time.time() - t0)/i

raw_input()