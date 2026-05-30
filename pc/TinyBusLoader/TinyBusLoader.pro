QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../QuCLib
INCLUDEPATH += $$PWD/../common

SOURCES += \
    ../QuCLib/source/cobs.cpp \
    ../QuCLib/source/crc.cpp \
    ../QuCLib/source/hexFileParser.cpp \
    ../common/protocol.cpp \
    ../common/connection/connection.cpp \
    ../common/connection/connectionBase.cpp \
    ../common/connection/connectionSerial.cpp \
    ../common/connection/connectionTcp.cpp \
    logic/busPassThrough.cpp \
    logic/device/update.cpp \
    logic/tinyBus.cpp \
    logic/device/device.cpp \
    main.cpp \
    ui/deviceInformationWidget.cpp \
    ui/deviceItemWidget.cpp \
    ui/eepromMemoryWidget.cpp \
    ui/flashMemoryWidget.cpp \
    ui/mainWindow.cpp \
    ui/memoryWidget.cpp \
    ui/queueItemWidget.cpp

HEADERS += \
    ../QuCLib/source/cobs.h \
    ../QuCLib/source/crc.h \
    ../QuCLib/source/hexFileParser.h \
    ../common/datatype.h \
    ../common/protocol.h \
    ../common/connection/connection.h \
    ../common/connection/connectionBase.h \
    ../common/connection/connectionSerial.h \
    ../common/connection/connectionTcp.h \
    logic/busPassThrough.h \
    logic/device/update.h \
    logic/tinyBus.h \
    logic/device/device.h \
    ui/colorPalette.h \
    ui/deviceInformationWidget.h \
    ui/deviceItemWidget.h \
    ui/eepromMemoryWidget.h \
    ui/flashMemoryWidget.h \
    ui/mainWindow.h \
    ui/memoryWidget.h \
    ui/queueItemWidget.h

FORMS += \
    ui/deviceInformationWidget.ui \
    ui/deviceItemWidget.ui \
    ui/eepromMemoryWidget.ui \
    ui/flashMemoryWidget.ui \
    ui/mainWindow.ui \
    ui/memoryWidget.ui \
    ui/queueItemWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
