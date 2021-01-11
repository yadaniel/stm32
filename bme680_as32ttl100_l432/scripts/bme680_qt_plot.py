#!/cygdrive/c/Python39/python
# checked with pyright 1.1.99

import re
import sys
import time
import serial
import datetime as dt
from dateutil import tz
from collections import deque
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import pyqtgraph as pg
from pyqtgraph import PlotWidget

if len(sys.argv) != 2:
    print("usage: bme680_plot.py <com>")
    sys.exit()

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

# 100ms
T = 100

# window length => number of samples to show
N = 100

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
        return (True,T,P/10,H,R/850e3*100)
    return (False,0,0,0,0)


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        # MainWindow.resize(929, 429)
        # MainWindow.resize(1429, 929)
        MainWindow.resize(1529, 929)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.graphicsView = PlotWidget(self.centralwidget)
        # self.graphicsView.setGeometry(QtCore.QRect(9, 9, 901, 321))
        self.graphicsView.setGeometry(QtCore.QRect(9, 9, 1401, 821))
        self.graphicsView.setObjectName("graphicsView")
        MainWindow.setCentralWidget(self.centralwidget)
        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "bme680"))

class ExampleApp(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.graphicsView.getAxis('left').setLabel('Temperature degC', color='#FFFF00')
        self.graphicsView.getAxis('right').setLabel('Pressure 10hPa', color='#FF0000')
        self.graphicsView.getAxis('left').setPen(pg.mkPen(color='y', width=2))
        self.graphicsView.getAxis('bottom').setLabel('time', 's')
        self.graphicsView.getAxis('bottom').setPen(pg.mkPen(color='y', width=2))
        self.graphicsView.showGrid(x=True, y=True)
        self.graphicsView.setYRange(10,110)
        # self.graphicsView.addLine(y=5,pen=pg.mkPen('y'))
        # self.graphicsView.addLine(y=7,pen=pg.mkPen('r'))
        self.graphicsView.addLegend()
        self.curveX1 = self.graphicsView.plot(name="temperature")
        self.curveX2 = self.graphicsView.plot(name="pressure")
        self.curveX3 = self.graphicsView.plot(name="humidity")
        self.curveX4 = self.graphicsView.plot(name="resistance")
        self.penX1 = pg.mkPen("y", width=3)
        self.penX2 = pg.mkPen("r", width=3)
        self.penX3 = pg.mkPen("b", width=3)
        self.penX4 = pg.mkPen("w", width=3)
        # timestamps
        t0 = time.time()
        self.ts = [t0-N+i for i in range(N)]
        self.x1 = [0.0 for i in range(N)]
        self.x2 = [0.0 for i in range(N)]
        self.x3 = [0.0 for i in range(N)]
        self.x4 = [0.0 for i in range(N)]
        # 
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(T)


    def updateplot(self):
        ok,s1,s2,s3,s4 = get_sample()
        if ok:
            self.ts.append(time.time())
            self.x1.append(s1)
            self.x2.append(s2)
            self.x3.append(s3)
            self.x4.append(s4)
            #
            self.ts = self.ts[-N:]
            self.x1 = self.x1[-N:]
            self.x2 = self.x2[-N:]
            self.x3 = self.x3[-N:]
            self.x4 = self.x4[-N:]
        #
        self.curveX1.setData(self.ts, self.x1, pen=self.penX1)
        self.curveX2.setData(self.ts, self.x2, pen=self.penX2)
        self.curveX3.setData(self.ts, self.x3, pen=self.penX3)
        self.curveX4.setData(self.ts, self.x4, pen=self.penX4)

def main():
    app = QtWidgets.QApplication(sys.argv)
    window = ExampleApp()
    window.show()
    app.exec_()

if __name__ == '__main__':
    main()


