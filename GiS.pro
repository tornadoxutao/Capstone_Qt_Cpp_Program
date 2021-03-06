#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T16:55:15
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GiS
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    Memory\Memory.cpp \
    CPU\CPUusage.cpp \
    CPU\CPUusagethread.cpp \
    Settings\settings.cpp \
    CPU\CPUtemp.cpp \
    CPU\CPUspeed.cpp \
    displaysettings.cpp \
    GPU\GPUtemp.cpp \
    OHM.cpp \
    Keyboard/keyboard.cpp \
    Keyboard/keyboardthread.cpp \
    Graph/Graph.cpp \
    Macro/Macro.cpp

HEADERS  += mainwindow.h \
    CPU\CPUusage.h \
    Memory\Memory.h \
    CPU\CPUusagethread.h \
    Settings\settings.h \
    CPU\CPUtemp.h \
    CPU\CPUspeed.h \
    displaysettings.h \
    GPU\GPUtemp.h \
    OHM.h \
    Keyboard/keyboard.h \
    Keyboard/keyboardthread.h \
    Graph/Graph.h \
    Macro/Macro.h

FORMS    += mainwindow.ui \
    displaysettings.ui

win32:RC_ICONS += 32x32icon.ico

win32: LIBS += -L$$PWD/Libraries/Sigar/sigar-bin/lib/ -lsigar-x86-winnt

INCLUDEPATH += $$PWD/Libraries/Sigar/sigar-bin/include
DEPENDPATH += $$PWD/Libraries/Sigar/sigar-bin/include

win32: LIBS += -L$$PWD/Libraries/Nvidia/x86/ -lnvapi

INCLUDEPATH += $$PWD/Libraries/Nvidia/x86
DEPENDPATH += $$PWD/Libraries/Nvidia/x86
