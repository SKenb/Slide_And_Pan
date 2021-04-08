from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QObject, QThread, pyqtSignal

from SerialFunctions import serialPortList
from Common import getVersionJsonEndpoint, getMockVersionJson

import time
import urllib.request, json 

class Ui_MainWindow(object):

    knownFirmwares = []

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(600, 400)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMinimumSize(QtCore.QSize(600, 400))
        MainWindow.setMaximumSize(QtCore.QSize(600, 400))
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.mdiArea_3 = QtWidgets.QMdiArea(self.centralwidget)
        self.mdiArea_3.setGeometry(QtCore.QRect(920, 0, 461, 551))
        brush = QtGui.QBrush(QtGui.QColor(177, 160, 160))
        brush.setStyle(QtCore.Qt.SolidPattern)
        self.mdiArea_3.setBackground(brush)
        self.mdiArea_3.setObjectName("mdiArea_3")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(30, 30, 400, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.comboBoxVersion = QtWidgets.QComboBox(self.centralwidget)
        self.comboBoxVersion.setGeometry(QtCore.QRect(30, 60, 531, 31))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.comboBoxVersion.setFont(font)
        self.comboBoxVersion.setObjectName("comboBoxVersion")
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(30, 190, 151, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.comboBoxSerial = QtWidgets.QComboBox(self.centralwidget)
        self.comboBoxSerial.setGeometry(QtCore.QRect(30, 220, 531, 31))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.comboBoxSerial.setFont(font)
        self.comboBoxSerial.setObjectName("comboBoxSerial")
        self.pushButtonStart = QtWidgets.QPushButton(self.centralwidget)
        self.pushButtonStart.setGeometry(QtCore.QRect(30, 300, 531, 61))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.pushButtonStart.setFont(font)
        self.pushButtonStart.setObjectName("pushButtonStart")
        self.label_3 = QtWidgets.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(30, 270, 151, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label_3.setFont(font)
        self.label_3.setObjectName("label_3")
        self.label_4 = QtWidgets.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(30, 370, 151, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_4.setFont(font)
        self.label_4.setObjectName("label_4")
        self.labelStatus = QtWidgets.QLabel(self.centralwidget)
        self.labelStatus.setGeometry(QtCore.QRect(80, 360, 501, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelStatus.setFont(font)
        self.labelStatus.setObjectName("labelStatus")
        self.label_5 = QtWidgets.QLabel(self.centralwidget)
        self.label_5.setGeometry(QtCore.QRect(50, 100, 41, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_5.setFont(font)
        self.label_5.setObjectName("label_5")
        self.label_6 = QtWidgets.QLabel(self.centralwidget)
        self.label_6.setGeometry(QtCore.QRect(50, 120, 51, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_6.setFont(font)
        self.label_6.setObjectName("label_6")
        self.label_7 = QtWidgets.QLabel(self.centralwidget)
        self.label_7.setGeometry(QtCore.QRect(50, 140, 81, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_7.setFont(font)
        self.label_7.setObjectName("label_7")
        self.label_8 = QtWidgets.QLabel(self.centralwidget)
        self.label_8.setGeometry(QtCore.QRect(50, 160, 51, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_8.setFont(font)
        self.label_8.setObjectName("label_8")
        self.labelFirmwareName = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareName.setGeometry(QtCore.QRect(160, 90, 501, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareName.setFont(font)
        self.labelFirmwareName.setObjectName("labelFirmwareName")
        self.labelFirmwareVersion = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareVersion.setGeometry(QtCore.QRect(160, 110, 501, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareVersion.setFont(font)
        self.labelFirmwareVersion.setObjectName("labelFirmwareVersion")
        self.labelFirmwareReleaseDate = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareReleaseDate.setGeometry(QtCore.QRect(160, 130, 501, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareReleaseDate.setFont(font)
        self.labelFirmwareReleaseDate.setObjectName("labelFirmwareReleaseDate")
        self.labelFirmwareNotes = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareNotes.setGeometry(QtCore.QRect(160, 150, 501, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareNotes.setFont(font)
        self.labelFirmwareNotes.setObjectName("labelFirmwareNotes")
        self.lineEditFirmwareKey = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEditFirmwareKey.setEnabled(False)
        self.lineEditFirmwareKey.setGeometry(QtCore.QRect(322, 100, 241, 21))
        self.lineEditFirmwareKey.setPlaceholderText("")
        self.lineEditFirmwareKey.setObjectName("lineEditFirmwareKey")
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "Slide & Pan - Bridge"))
        self.label.setText(_translate("MainWindow", "Firmware - Version"))
        self.label_2.setText(_translate("MainWindow", "Serial"))
        self.pushButtonStart.setText(_translate("MainWindow", "Start Programming"))
        self.label_3.setText(_translate("MainWindow", "Program"))
        self.label_4.setText(_translate("MainWindow", "Status"))
        self.labelStatus.setText(_translate("MainWindow", "Status"))
        self.label_5.setText(_translate("MainWindow", "Name:"))
        self.label_6.setText(_translate("MainWindow", "Version:"))
        self.label_7.setText(_translate("MainWindow", "Release Date:"))
        self.label_8.setText(_translate("MainWindow", "Notes:"))
        self.labelFirmwareName.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareVersion.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareReleaseDate.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareNotes.setText(_translate("MainWindow", "Status"))

    def changeUi(self):

        self.comboBoxVersion.currentIndexChanged.connect(self.changeFirmwareInfoText)

    def changeFirmwareInfoText(self, index):

        if len(self.knownFirmwares) < index:

            self.labelFirmwareName.setText("")
            self.labelFirmwareVersion.setText("")
            self.labelFirmwareReleaseDate.setText("")
            self.labelFirmwareNotes.setText("")

        else:

            try:
                
                self.labelFirmwareName.setText(self.knownFirmwares[index]["Name"])
                self.labelFirmwareVersion.setText(self.knownFirmwares[index]["Version"])
                self.labelFirmwareReleaseDate.setText(self.knownFirmwares[index]["ReleaseDate"])
                self.labelFirmwareNotes.setText(self.knownFirmwares[index]["Notes"])

                if self.knownFirmwares[index]["IsKeyRequired"] >= 1:
                    self.lineEditFirmwareKey.setEnabled(True)
                    self.lineEditFirmwareKey.setText("Firmware-Key")
                else:
                    self.lineEditFirmwareKey.setEnabled(False)
                    self.lineEditFirmwareKey.setText("")

            except Exception:

                self.setStatus("Error occurred during Firmware info update :/")


    def setStatus(self, status):
        self.labelStatus.setText(status)

    def updateSerialPorts(self):

        self.updateSerialPortsThread = QThread()
        self.updateSerialPortsWorker = UpdateSerialPortsWorker()
        self.updateSerialPortsWorker.moveToThread(self.updateSerialPortsThread)

        self.updateSerialPortsThread.started.connect(self.updateSerialPortsWorker.run)
        self.updateSerialPortsWorker.finished.connect(self.updateSerialPortsThread.quit)
        self.updateSerialPortsWorker.finished.connect(self.updateSerialPortsWorker.deleteLater)
        self.updateSerialPortsThread.finished.connect(self.updateSerialPortsThread.deleteLater)

        self.updateSerialPortsWorker.progress.connect(lambda port: self.comboBoxSerial.addItem(port))
        self.updateSerialPortsWorker.statusUpdate.connect(lambda status: self.setStatus(status))

        self.updateSerialPortsThread.start()

        # Final resets
        self.updateSerialPortsThread.finished.connect(lambda: self.setStatus("COM Ports updated :)"))

    def addNewFirmware(self, firmware):

        self.knownFirmwares.append(firmware)
        self.comboBoxVersion.addItem(firmware["Name"] + " - (Version: " + firmware["Version"] +  ")")
        self.changeFirmwareInfoText(self.comboBoxVersion.currentIndex())

    def updateAvailableFirmwares(self):

        self.updateAvailableFirmwaresThread = QThread()
        self.updateAvailableFirmwaresWorker = UpdateAvailableFirmwaresWorker()
        self.updateAvailableFirmwaresWorker.moveToThread(self.updateAvailableFirmwaresThread)

        self.updateAvailableFirmwaresThread.started.connect(self.updateAvailableFirmwaresWorker.run)
        self.updateAvailableFirmwaresWorker.finished.connect(self.updateAvailableFirmwaresThread.quit)
        self.updateAvailableFirmwaresWorker.finished.connect(self.updateAvailableFirmwaresWorker.deleteLater)
        self.updateAvailableFirmwaresThread.finished.connect(self.updateAvailableFirmwaresThread.deleteLater)

        self.updateAvailableFirmwaresWorker.newFirmware.connect(self.addNewFirmware)
        self.updateAvailableFirmwaresWorker.statusUpdate.connect(lambda status: self.setStatus(status))

        self.updateAvailableFirmwaresThread.start()

        # Final resets
        self.updateAvailableFirmwaresThread.finished.connect(lambda: self.setStatus("Updated available Firmwares :)"))



class UpdateSerialPortsWorker(QObject):
    finished = pyqtSignal()
    progress = pyqtSignal(object)
    statusUpdate = pyqtSignal(object)
    knownPorts = []

    def run(self):
        self.statusUpdate.emit("Search serial ports...")

        while True:

            for port in serialPortList():
                if port not in self.knownPorts:
                    self.statusUpdate.emit("Found new port: " + port)
                    self.progress.emit(port)

                    self.knownPorts.append(port)

            time.sleep(2)

        self.finished.emit()

class UpdateAvailableFirmwaresWorker(QObject):
    finished = pyqtSignal()
    newFirmware = pyqtSignal(object)
    statusUpdate = pyqtSignal(object)
    knownFirmwares = []

    def run(self):
        self.statusUpdate.emit("Search available Firmwares...")

        while True:

            with urllib.request.urlopen(getVersionJsonEndpoint()) as url:
                #data = json.loads(url.read().decode())

                data = json.loads(getMockVersionJson())

                for firmware in data["Firmwares"]:
                    firmwareName = firmware["Name"]

                    if firmwareName not in self.knownFirmwares:

                        self.knownFirmwares.append(firmwareName)
                        self.statusUpdate.emit("Found new Firmware (" + firmwareName + ")")
                        self.newFirmware.emit(firmware)


            time.sleep(20)

        self.finished.emit()