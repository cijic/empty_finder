#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T22:54:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EmptyFinder
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	dirhandler.cpp

HEADERS  += mainwindow.h \
	dirhandler.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    icon.ico
