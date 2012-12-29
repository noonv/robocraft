#-------------------------------------------------
#
# Project created by QtCreator 2012-11-30T12:20:31
#
#-------------------------------------------------

QT       += core gui

TARGET = qserial_plot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    plot.cpp \
    serialreader.cpp \
#    curvedata.cpp \
#    signaldata.cpp
    curvedata.cpp \
    serial.cpp

HEADERS  += mainwindow.h \
    socketreader.h \
    plot.h \
    serialreader.h \
#    curvedata.h \
#    signaldata.h
    curvedata.h \
    serial.h \
    definitions.h

FORMS    += mainwindow.ui \

#INCLUDEPATH +=
#LIBS +=

include( qwt.pri )
