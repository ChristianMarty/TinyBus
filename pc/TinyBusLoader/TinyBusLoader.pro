QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../QuCLib/source/cobs.cpp \
    ../QuCLib/source/crc.cpp \
    ../QuCLib/source/hexFileParser.cpp \
    logic/busPassThrough.cpp \
    logic/connection.cpp \
    logic/connectionSerial.cpp \
    logic/connectionTcp.cpp \
    logic/tinyBus.cpp \
    logic/device.cpp \
    main.cpp \
    ui/deviceInformationWidget.cpp \
    ui/deviceItemWidget.cpp \
    ui/eepromMemoryWidget.cpp \
    ui/mainWindow.cpp \
    ui/memoryWidget.cpp \
    ui/queueItemWidget.cpp

HEADERS += \
    ../QuCLib/source/cobs.h \
    ../QuCLib/source/crc.h \
    ../QuCLib/source/hexFileParser.h \
    logic/busPassThrough.h \
    logic/connection.h \
    logic/connectionSerial.h \
    logic/connectionTcp.h \
    logic/tinyBus.h \
    logic/device.h \
    ui/colorPalette.h \
    ui/deviceInformationWidget.h \
    ui/deviceItemWidget.h \
    ui/eepromMemoryWidget.h \
    ui/mainWindow.h \
    ui/memoryWidget.h \
    ui/queueItemWidget.h

FORMS += \
    ui/deviceInformationWidget.ui \
    ui/deviceItemWidget.ui \
    ui/eepromMemoryWidget.ui \
    ui/mainWindow.ui \
    ui/memoryWidget.ui \
    ui/queueItemWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
