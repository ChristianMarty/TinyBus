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
    logic/test.cpp \
    logic/testItem/readApplicationName.cpp \
    logic/testItem/readApplicationVersion.cpp \
    logic/testItem/testItemBase.cpp \
    main.cpp \
    ui/mainWindow.cpp \
    ui/testItemWidget.cpp

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
    logic/device/datatype.h \
    logic/test.h \
    logic/testItem/readApplicationName.h \
    logic/testItem/readApplicationVersion.h \
    logic/testItem/testItemBase.h \
    ui/colorPalette.h \
    ui/mainWindow.h \
    ui/testItemWidget.h

FORMS += \
    ui/mainWindow.ui \
    ui/testItemWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
