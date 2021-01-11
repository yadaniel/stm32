#!/cygdrive/c/Python39/python
# checked with pyright 1.1.99

import re
import sys
import time
import serial
import random
import datetime as dt
from dateutil import tz
import matplotlib.pylab as plt
from matplotlib.animation import FuncAnimation

if len(sys.argv) != 2:
    print("usage: bme680_plot.py <com>")
    sys.exit()

# set correct timezone
TZ = tz.gettz("Europe/Berlin")

# logfile
# log = None
log = open("bme680_%s.log" % dt.datetime.now(TZ).strftime("%Y_%m_%d_%H_%M_%S"), "w")

COM_PORT = sys.argv[1]

try:
    com = serial.Serial(port=COM_PORT, baudrate=2400, timeout=0.5)
except serial.SerialException as e:
    if "FileNotFound" in str(e):
        print(f"{COM_PORT} does not exist")
    elif "PermissionError" in str(e):
        print(f"{COM_PORT} already opened")
    else:
        print(f"serial exception with {COM_PORT} => {e}")
    sys.exit()
except Exception as e:
    print(f"exception => {e}")
    sys.exit()

pattern = re.compile(r'^BME680 read ... T:(?P<T>.*?)degC, P:(?P<P>.*?)hPa, H:(?P<H>.*?)[%]rH, G:(?P<R>.*?)ohms.*$', re.M|re.DOTALL)

# Create figure for plotting
fig = plt.figure("bme680 sensor")
fig.patch.set_facecolor('xkcd:mint green')
ax1 = fig.add_subplot(4,1,1)
ax2 = fig.add_subplot(4,1,2)
ax3 = fig.add_subplot(4,1,3)
ax4 = fig.add_subplot(4,1,4)

# 1s
T = 50

# window length => number of samples to show
N = 60
t0 = time.time()
# ts = [dt.datetime.fromtimestamp(t0-N+i).strftime('%H:%M:%S.%f') for i in range(N)]
ts = [dt.datetime.fromtimestamp(t0-N+i).strftime('%H:%M:%S.%f')[:-5] for i in range(N)]

x1 = [0 for i in range(N)]
x2 = [0 for i in range(N)]
x3 = [0 for i in range(N)]
x4 = [0 for i in range(N)]

# read data from bme680
def get_sample():
    line = com.read(1000)
    # print(f"{line}")
    m = pattern.match(line.decode("ascii"))
    if m:
        T = float(m.group("T"))
        P = float(m.group("P"))
        H = float(m.group("H"))
        R = int(m.group("R"))
        print(f"{T:.2f}, {P:.2f}, {H:.2f}, {R}", flush=True)
        if log:
            log.write(f"{dt.datetime.now(TZ).strftime('%H_%M_%S.%f')}, {T:.2f}, {P:.2f}, {H:.2f}, {R}\n")
            log.flush()
        return (True,T,P,H,R)
    return (False,0,0,0,0)

# This function is called periodically from FuncAnimation
def animate(i, ts, x1, x2, x3, x4):
    ok,s1,s2,s3,s4 = get_sample()
    if ok:
        # ts.append(dt.datetime.now(TZ).strftime('%H:%M:%S.%f'))
        ts.append(dt.datetime.now(TZ).strftime('%H:%M:%S.%f')[:-5])
        x1.append(s1)
        x2.append(s2)
        x3.append(s3)
        x4.append(s4)
        # limit lists to N items
        ts = ts[-N:]
        x1 = x1[-N:]
        x2 = x2[-N:]
        x3 = x3[-N:]
        x4 = x4[-N:]

    # Draw x and y lists
    ax1.clear()
    ax1.plot(ts, x1, "y")
    ax1.set_ylim(5, 35)
    ax1.set_ylabel('temperature [gradC]')
    ax1.grid()

    ax2.clear()
    ax2.plot(ts, x2, "r")
    ax2.set_ylim(900, 1100)
    ax2.set_ylabel('pressure [hPa]')
    ax2.grid()

    ax3.clear()
    ax3.plot(ts, x3, "g")
    ax3.set_ylim(10, 100)
    ax3.set_ylabel('humidity [%rH]')
    ax3.grid()

    ax4.clear()
    ax4.plot(ts, x4, "b")
    ax4.set_ylim(10_000, 800_000)
    ax4.set_ylabel('IAQ [ohm]')
    ax4.grid()

    plt.setp(ax1.get_xticklabels(), visible=False)
    plt.setp(ax2.get_xticklabels(), visible=False)
    plt.setp(ax3.get_xticklabels(), visible=False)
    plt.setp(ax4.get_xticklabels(), visible=True)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.tight_layout()

# Set up plot to call animate() function periodically
ani = FuncAnimation(fig, func=animate, fargs=(ts, x1, x2, x3, x4), interval=T)
plt.show()

