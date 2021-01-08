#!/cygdrive/c/Python39/python
# checked with pyright 1.1.99

import random
import time, sys
import datetime as dt
from dateutil import tz
from collections import deque
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
import pyqtgraph as pg
from pyqtgraph import PlotWidget

# set correct timezone
TZ = tz.gettz("Europe/Berlin")

# logfile
# log = None
log = open("sensor_%s.log" % dt.datetime.now(TZ).strftime("%Y_%m_%d_%H_%M_%S"), "w")

# 100ms
T = 100

# window length => number of samples to show
N = 100

# simulate read data from sensors
def get_sample():
    s1 = random.randint(-100,0)
    s2 = random.randint(-50,0)
    s3 = random.randint(0,50)
    s4 = random.randint(0,100)
    if log:
        log.write(f"{dt.datetime.now(TZ).strftime('%H_%M_%S.%f')}, {s1:.2f}, {s2:.2f}, {s3:.2f}, {s4:.2f}\n")
        log.flush()
    return (s1,s2,s3,s4)

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
        MainWindow.setWindowTitle(_translate("MainWindow", "multisensor animated graph"))

class ExampleApp(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.graphicsView.getAxis('left').setLabel('x1, x2, x3, x4', color='#FFFF00')
        self.graphicsView.getAxis('left').setPen(pg.mkPen(color='y', width=2))
        self.graphicsView.getAxis('bottom').setLabel('time', '[s]')
        self.graphicsView.getAxis('bottom').setPen(pg.mkPen(color='y', width=2))
        self.graphicsView.showGrid(x=True, y=True)
        self.graphicsView.setYRange(-100,100)
        # self.graphicsView.addLine(y=5,pen=pg.mkPen('y'))
        # self.graphicsView.addLine(y=7,pen=pg.mkPen('r'))
        self.graphicsView.addLegend()
        self.curveX1 = self.graphicsView.plot(name="x1")
        self.curveX2 = self.graphicsView.plot(name="x2")
        self.curveX3 = self.graphicsView.plot(name="x3")
        self.curveX4 = self.graphicsView.plot(name="x4")
        self.penX1 = pg.mkPen("y", width=3)
        self.penX2 = pg.mkPen("r", width=3)
        self.penX3 = pg.mkPen("g", width=3)
        self.penX4 = pg.mkPen("b", width=3)
        # timestamps
        t0 = time.time()
        self.ts = [t0-N+i for i in range(N)]
        self.x1 = [0 for i in range(N)]
        self.x2 = [0 for i in range(N)]
        self.x3 = [0 for i in range(N)]
        self.x4 = [0 for i in range(N)]
        # 
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(T)

    def updateplot(self):
        s1,s2,s3,s4 = get_sample()
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

