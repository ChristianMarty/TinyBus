QT += core
QT += gui
QT += widgets
QT += network
QT += serialport

CONFIG += c++23
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

APPLICATION_NAME = TinyBusLoader
APPLICATION_DESCRIPTION = TinyBus
APPLICATION_COPYRIGHT = Christian Marty

VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0
VERSION_BUILD = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}.$${VERSION_BUILD}
DEFINES += APPLICATION_VERSION=\\\"$${VERSION}\\\"

TARGET = $${APPLICATION_NAME}

#RC_ICONS = $$PWD/logo.png

INCLUDEPATH += $$PWD/../QuCLib
INCLUDEPATH += $$PWD/../common

SOURCES += \
    ../QuCLib/source/cobs.cpp \
    ../QuCLib/source/crc.cpp \
    ../QuCLib/source/hexFileParser.cpp \
    ../QuCLib/source/uiComponents/memoryTextWidget.cpp \
    ../common/connectionHandler.cpp \
    ../common/protocol.cpp \
    ../common/connection/connection.cpp \
    ../common/connection/connectionBase.cpp \
    ../common/connection/connectionSerial.cpp \
    ../common/connection/connectionTcp.cpp \
    logic/busMonitorModel.cpp \
    ../common/busPassThrough.cpp \
    logic/device/update.cpp \
    logic/tinyBus.cpp \
    logic/device/device.cpp \
    main.cpp \
    ui/busMonitorWidget.cpp \
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
    ../QuCLib/source/uiComponents/memoryTextWidget.h \
    ../QuCLib/source/uiComponents/uiDatatypes.h \
    ../common/connectionHandler.h \
    ../common/datatype.h \
    ../common/protocol.h \
    ../common/connection/connection.h \
    ../common/connection/connectionBase.h \
    ../common/connection/connectionSerial.h \
    ../common/connection/connectionTcp.h \
    logic/busMonitorModel.h \
    ../common/busPassThrough.h \
    logic/device/update.h \
    logic/tinyBus.h \
    logic/device/device.h \
    ui/busMonitorWidget.h \
    ui/colorPalette.h \
    ui/deviceInformationWidget.h \
    ui/deviceItemWidget.h \
    ui/eepromMemoryWidget.h \
    ui/flashMemoryWidget.h \
    ui/mainWindow.h \
    ui/memoryWidget.h \
    ui/queueItemWidget.h

FORMS += \
    ui/busMonitorWidget.ui \
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

win32:CONFIG(release, debug|release){
    # add executable meta data
    QMAKE_TARGET_COMPANY = $$COMPANY_NAME
    QMAKE_TARGET_PRODUCT = $$APPLICATION_NAME
    QMAKE_TARGET_DESCRIPTION = $$APPLICATION_DESCRIPTION
    QMAKE_TARGET_COPYRIGHT = $$APPLICATION_COPYRIGHT

    RELEASE_FOLDER = $$PWD/../build/Desktop_Qt_6_11_1_MinGW_64_bit_Release/release
    DISTRIBUTION_FOLDER = $$PWD/../build/distribution

    RELEASE_FILES = \
            $${RELEASE_FOLDER}/$${APPLICATION_NAME}.exe

    distribution.files = $${RELEASE_FILES}
    distribution.path = $${DISTRIBUTION_FOLDER}

    COPIES += distribution

    TOOLCHAIN =  $$[QT_INSTALL_PREFIX]/bin/
    DEPLOY_MAIN = $${TOOLCHAIN}windeployqt6.exe $${DISTRIBUTION_FOLDER}/$${APPLICATION_NAME}.exe

    system($$DEPLOY_MAIN)
}

