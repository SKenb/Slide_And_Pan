import sys

from MainWindow import Ui_MainWindow
from PyQt5 import QtCore, QtGui, QtWidgets

#import esptool
import os

if __name__ == "__main__":

    app = QtWidgets.QApplication(sys.argv)
    QMainWindow = QtWidgets.QMainWindow()

    window = Ui_MainWindow()
    window.setupUi(QMainWindow)
    window.changeUi()

    QMainWindow.show()

    window.updateSerialPorts()
    window.updateAvailableFirmwares()

    sys.exit(app.exec_())
    



    