#-------------------------------------------------
#
# Project created by QtCreator 2021-10-11T10:34:45
#
#-------------------------------------------------

QT       += core gui network sql winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TConPro
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
QMAKE_CXXFLAGS_RELEASE = -Od -ZI -MD
QMAKE_LFLAGS_RELEASE = /DEBUG /INCREMENTAL:NO

SOURCES += \
        aboutwgt.cpp \
        calibrationwidget.cpp \
        databasewgt.cpp \
        gdatafactory.cpp \
        imagewidget.cpp \
        imessagebox.cpp \
        main.cpp \
        mesprocess.cpp \
        paraconfigwgt.cpp \
        title/TitleBar.cpp \
        title/titlewidget.cpp \
        topbottomwidget.cpp \
        topcentralwidget.cpp \
        topmainwindow.cpp \
        toprightwidget.cpp \
        topwidget.cpp \
        udpservice.cpp \
        vision.cpp \
        welcomedlg.cpp

HEADERS += \
        aboutwgt.h \
        calibrationwidget.h \
        databasewgt.h \
        gdatafactory.h \
        imagewidget.h \
        imessagebox.h \
        mesprocess.h \
        paraconfigwgt.h \
        title/TitleBar.h \
        title/titlewidget.h \
        topbottomwidget.h \
        topcentralwidget.h \
        topmainwindow.h \
        toprightwidget.h \
        topwidget.h \
        udpservice.h \
        vision.h \
        welcomedlg.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pro.qrc

INCLUDEPATH += .\title
INCLUDEPATH += .\include
INCLUDEPATH += .\include\halconcpp
INCLUDEPATH += .\dbandqsloginclude


DEFINES += QSLOG_IS_SHARED_LIBRARY_IMPORT
DEFINES += QS_LOG_LINE_NUMBERS
LIBS += -L$$PWD\lib\x64\ -lQsLog2
LIBS += -L$$PWD\lib\x64\ -lDMDBAccess
LIBS += -L$$PWD\lib\x64\ -lhalconcpp
LIBS += -L$$PWD\lib\x64\ -lhalcon
LIBS += User32.LIB
LIBS += Gdi32.LIB
