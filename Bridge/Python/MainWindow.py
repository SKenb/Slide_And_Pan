from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QObject, QThread, pyqtSignal

from SerialFunctions import serialPortList
from Common import getFirmwareListEndpoint, getMockVersionJson, getFirmwareEndpoint

import os
import time
import urllib.request, json
import esptool
import subprocess

firmwareToProgramm = None
firmwareKey = None
serialPortToUse = None

class Ui_MainWindow(object):

    knownFirmwares = []

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMinimumSize(QtCore.QSize(800, 600))
        MainWindow.setMaximumSize(QtCore.QSize(800, 600))
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.mdiArea_3 = QtWidgets.QMdiArea(self.centralwidget)
        self.mdiArea_3.setGeometry(QtCore.QRect(920, 0, 461, 551))
        brush = QtGui.QBrush(QtGui.QColor(177, 160, 160))
        brush.setStyle(QtCore.Qt.SolidPattern)
        self.mdiArea_3.setBackground(brush)
        self.mdiArea_3.setObjectName("mdiArea_3")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(20, 190, 401, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.comboBoxVersion = QtWidgets.QComboBox(self.centralwidget)
        self.comboBoxVersion.setGeometry(QtCore.QRect(20, 220, 751, 31))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.comboBoxVersion.setFont(font)
        self.comboBoxVersion.setObjectName("comboBoxVersion")
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(20, 360, 471, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")
        self.comboBoxSerial = QtWidgets.QComboBox(self.centralwidget)
        self.comboBoxSerial.setGeometry(QtCore.QRect(20, 390, 751, 31))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.comboBoxSerial.setFont(font)
        self.comboBoxSerial.setObjectName("comboBoxSerial")
        self.pushButtonStart = QtWidgets.QPushButton(self.centralwidget)
        self.pushButtonStart.setGeometry(QtCore.QRect(20, 500, 751, 61))
        font = QtGui.QFont()
        font.setPointSize(14)
        self.pushButtonStart.setFont(font)
        self.pushButtonStart.setObjectName("pushButtonStart")
        self.label_3 = QtWidgets.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(20, 430, 491, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(14)
        self.label_3.setFont(font)
        self.label_3.setObjectName("label_3")
        self.label_4 = QtWidgets.QLabel(self.centralwidget)
        self.label_4.setGeometry(QtCore.QRect(20, 570, 151, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_4.setFont(font)
        self.label_4.setObjectName("label_4")
        self.labelStatus = QtWidgets.QLabel(self.centralwidget)
        self.labelStatus.setGeometry(QtCore.QRect(70, 560, 1400, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelStatus.setFont(font)
        self.labelStatus.setObjectName("labelStatus")
        self.label_5 = QtWidgets.QLabel(self.centralwidget)
        self.label_5.setGeometry(QtCore.QRect(40, 260, 41, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_5.setFont(font)
        self.label_5.setObjectName("label_5")
        self.label_6 = QtWidgets.QLabel(self.centralwidget)
        self.label_6.setGeometry(QtCore.QRect(40, 280, 51, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_6.setFont(font)
        self.label_6.setObjectName("label_6")
        self.label_7 = QtWidgets.QLabel(self.centralwidget)
        self.label_7.setGeometry(QtCore.QRect(40, 300, 81, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_7.setFont(font)
        self.label_7.setObjectName("label_7")
        self.label_8 = QtWidgets.QLabel(self.centralwidget)
        self.label_8.setGeometry(QtCore.QRect(40, 320, 51, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_8.setFont(font)
        self.label_8.setObjectName("label_8")
        self.labelFirmwareName = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareName.setGeometry(QtCore.QRect(150, 250, 650, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareName.setFont(font)
        self.labelFirmwareName.setObjectName("labelFirmwareName")
        self.labelFirmwareVersion = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareVersion.setGeometry(QtCore.QRect(150, 270, 650, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareVersion.setFont(font)
        self.labelFirmwareVersion.setObjectName("labelFirmwareVersion")
        self.labelFirmwareReleaseDate = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareReleaseDate.setGeometry(QtCore.QRect(150, 290, 650, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareReleaseDate.setFont(font)
        self.labelFirmwareReleaseDate.setObjectName("labelFirmwareReleaseDate")
        self.labelFirmwareNotes = QtWidgets.QLabel(self.centralwidget)
        self.labelFirmwareNotes.setGeometry(QtCore.QRect(150, 310, 650, 41))
        font = QtGui.QFont()
        font.setFamily("BuRGfont TTF")
        font.setPointSize(8)
        self.labelFirmwareNotes.setFont(font)
        self.labelFirmwareNotes.setObjectName("labelFirmwareNotes")
        self.lineEditFirmwareKey = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEditFirmwareKey.setEnabled(False)
        self.lineEditFirmwareKey.setGeometry(QtCore.QRect(530, 260, 241, 21))
        self.lineEditFirmwareKey.setPlaceholderText("")
        self.lineEditFirmwareKey.setObjectName("lineEditFirmwareKey")
        self.label_9 = QtWidgets.QLabel(self.centralwidget)
        self.label_9.setGeometry(QtCore.QRect(490, 260, 41, 21))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(10)
        self.label_9.setFont(font)
        self.label_9.setObjectName("label_9")
        self.label_10 = QtWidgets.QLabel(self.centralwidget)
        self.label_10.setGeometry(QtCore.QRect(20, 0, 471, 51))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(20)
        font.setBold(True)
        font.setWeight(75)
        self.label_10.setFont(font)
        self.label_10.setObjectName("label_10")
        self.label_11 = QtWidgets.QLabel(self.centralwidget)
        self.label_11.setGeometry(QtCore.QRect(20, 460, 751, 31))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_11.setFont(font)
        self.label_11.setAutoFillBackground(True)
        self.label_11.setFrameShape(QtWidgets.QFrame.Panel)
        self.label_11.setObjectName("label_11")
        self.label_12 = QtWidgets.QLabel(self.centralwidget)
        self.label_12.setGeometry(QtCore.QRect(40, 50, 731, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_12.setFont(font)
        self.label_12.setObjectName("label_12")
        self.label_13 = QtWidgets.QLabel(self.centralwidget)
        self.label_13.setGeometry(QtCore.QRect(40, 70, 741, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_13.setFont(font)
        self.label_13.setObjectName("label_13")
        self.label_14 = QtWidgets.QLabel(self.centralwidget)
        self.label_14.setGeometry(QtCore.QRect(40, 90, 731, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label_14.setFont(font)
        self.label_14.setObjectName("label_14")
        self.labelLink = QtWidgets.QLabel(self.centralwidget)
        self.labelLink.setGeometry(QtCore.QRect(40, 120, 731, 31))
        font = QtGui.QFont()
        font.setPointSize(12)
        self.labelLink.setFont(font)
        self.labelLink.setObjectName("labelLink")
        self.label_16 = QtWidgets.QLabel(self.centralwidget)
        self.label_16.setGeometry(QtCore.QRect(650, 580, 221, 16))
        self.label_16.setObjectName("label_16")
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
        self.label_7.setText(_translate("MainWindow", "Releasae Date:"))
        self.label_8.setText(_translate("MainWindow", "Notes:"))
        self.labelFirmwareName.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareVersion.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareReleaseDate.setText(_translate("MainWindow", "Status"))
        self.labelFirmwareNotes.setText(_translate("MainWindow", "Status"))
        self.label_9.setText(_translate("MainWindow", "Key:"))
        self.label_10.setText(_translate("MainWindow", "Slide & Pan - Bridge"))
        self.label_11.setText(_translate("MainWindow", "Can be used to flash a Wemos D1 Mini Pro at your own risk. No warranty or support is given for damage or incorrect function. All rights reserved."))
        self.label_12.setText(_translate("MainWindow", "One can use this program to initialize or update his/her slider."))
        self.label_13.setText(_translate("MainWindow", "Thereby a Firmware version can be selected and the serial port for programming selected."))
        self.label_14.setText(_translate("MainWindow", "All available Firmware-Versions and functionalities can be found here:"))
        self.labelLink.setText(_translate("MainWindow", "Link"))
        self.label_16.setText(_translate("MainWindow", "Version.: 1.0.1"))

    def changeUi(self):

        self.comboBoxVersion.currentIndexChanged.connect(self.changeFirmwareInfoText)
        self.pushButtonStart.clicked.connect(self.startProgrammSlider)

        self.pushButtonStart.setEnabled(False)

        self.labelLink.setText('<a href="https://sebastianknoll.net/slider/">Slide & Pan</a> <a href="https://sebastianknoll.net/slider/handbook">Handbook</a> <a href="https://sebastianknoll.net/slider/Software#Firmwares">Firmwares</a>')
        self.labelLink.setOpenExternalLinks(True)

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

                isKeyRequired = self.knownFirmwares[index]["IsKeyRequired"]
                if isKeyRequired is not None and int(isKeyRequired)  >= 1:
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
        self.updateSerialPortsThread.finished.connect(lambda: self.setStatus("COM Ports updated :)") or self.pushButtonStart.setEnabled(True))

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

    def startProgrammSlider(self):
        global firmwareToProgramm, serialPortToUse, firmwareKey

        self.pushButtonStart.setEnabled(False)

        firmwareToProgramm = self.knownFirmwares[self.comboBoxVersion.currentIndex()]
        serialPortToUse = self.comboBoxSerial.currentText()
        firmwareKey = self.lineEditFirmwareKey.text()

        self.programmSliderThread = QThread()
        self.programmSliderWorker = ProgramSliderWorker()
        self.programmSliderWorker.moveToThread(self.programmSliderThread)

        self.programmSliderThread.started.connect(self.programmSliderWorker.run)
        self.programmSliderWorker.finished.connect(self.programmSliderThread.quit)
        self.programmSliderWorker.finished.connect(self.programmSliderWorker.deleteLater)
        self.programmSliderThread.finished.connect(self.programmSliderThread.deleteLater)
        self.programmSliderThread.finished.connect(lambda : self.pushButtonStart.setEnabled(True))

        self.programmSliderWorker.errorUpdate.connect(lambda error: self.setStatus(error))
        self.programmSliderWorker.statusUpdate.connect(lambda status: self.setStatus(status))

        self.programmSliderThread.start()



class UpdateSerialPortsWorker(QObject):
    finished = pyqtSignal()
    progress = pyqtSignal(object)
    statusUpdate = pyqtSignal(object)
    knownPorts = []

    def run(self):
        self.statusUpdate.emit("Search serial ports...")

        #while True:

        try:
            for port in serialPortList():
                if port not in self.knownPorts:
                    self.statusUpdate.emit("Found new port: " + port)
                    self.progress.emit(port)

                    self.knownPorts.append(port)
        except Exception:
            self.statusUpdate.emit("Serial port update failed :0 - 0x01")

        #    time.sleep(2)

        self.finished.emit()

class UpdateAvailableFirmwaresWorker(QObject):
    finished = pyqtSignal()
    newFirmware = pyqtSignal(object)
    statusUpdate = pyqtSignal(object)
    knownFirmwares = []

    def run(self):
        self.statusUpdate.emit("Search available Firmwares...")

        while True:

            try:
                with urllib.request.urlopen(getFirmwareListEndpoint()) as url:
                    data = json.loads(url.read().decode())

                    #data = json.loads(getMockVersionJson())

                    for firmware in data["Firmwares"]:
                        firmwareName = firmware["Name"]

                        if firmwareName not in self.knownFirmwares:

                            self.knownFirmwares.append(firmwareName)
                            self.statusUpdate.emit("Found new Firmware (" + firmwareName + ")")
                            self.newFirmware.emit(firmware)

            except Exception:
                self.statusUpdate.emit("Firmware search failed :/ - 0x02")

            time.sleep(20)

        self.finished.emit()

class ProgramSliderWorker(QObject):
    finished = pyqtSignal()
    statusUpdate = pyqtSignal(object)
    errorUpdate = pyqtSignal(object)

    def run(self):
        
        try:
            self.statusUpdate.emit("Start programming...")

            localFilename = './.firmware.bin'
            # Clean update
            self.statusUpdate.emit("Clean for update")
            if os.path.exists(localFilename):
                os.remove(localFilename)
            
            # Download firmware
            self.statusUpdate.emit("Download requested firmware")

            filedata = urllib.request.urlopen(getFirmwareEndpoint(firmwareToProgramm["Name"], firmwareKey))
            firmwareData = filedata.read()

            if len(firmwareData) <= 2000:
                if "Error" in str(firmwareData):
                    errorData = str(firmwareData).replace('b', '').replace('\'', '')
                    errorData = json.loads(errorData)
                    self.errorUpdate.emit(errorData["Error"])
                    self.finished.emit()
                    return
                else:
                    self.errorUpdate.emit("No valid firmware received :0")
                    self.finished.emit()
                    return
            
            with open(localFilename, 'wb') as f:
                f.write(firmwareData)


            # Program
            self.statusUpdate.emit("Program the Slider")

            try:
                from subprocess import DEVNULL
            except ImportError:
                DEVNULL = os.open(os.devnull, os.O_RDWR)

            result = str(subprocess.check_output("esptool.py --chip auto --port " + serialPortToUse + " write_flash -fm dio -fs 16MB 0x00000 " + localFilename, shell=True, stdin=DEVNULL, stderr=DEVNULL))

            # Clean update
            self.statusUpdate.emit("Clean up")
            if os.path.exists(localFilename):
                os.remove(localFilename)

            if("Hash of data verified" in result):
                self.statusUpdate.emit("Programming finished :D")
            else:
                self.errorUpdate.emit("Something went wrong when programming the Slider :/")

        except Exception as e:
             # Clean update
            self.statusUpdate.emit("Clean up")
            if os.path.exists(localFilename):
                os.remove(localFilename)

            self.errorUpdate.emit("Something went wrong 0.o - 0x3")

        self.finished.emit()