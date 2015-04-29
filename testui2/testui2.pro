#-------------------------------------------------
#
# Project created by QtCreator 2015-01-29T23:29:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testui2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        freqampdialog.cpp \
    meshdialog.cpp

HEADERS  += mainwindow.h\
            freqampdialog.h \
    meshdialog.h

FORMS    += mainwindow.ui\
            freqampdialog.ui \
    meshdialog.ui
