#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T18:51:56
#
#-------------------------------------------------

QT       += core gui

TARGET = RadCon
TEMPLATE = app


SOURCES += main.cpp\
        radiocontrol.cpp \
    globals.cpp \
    connectiondialog.cpp

QMAKE_CXXFLAGS += -std=c++0x -Werror

HEADERS  += radiocontrol.h \
    globals.h \
    connectiondialog.h

FORMS    += radiocontrol.ui \
    connectiondialog.ui

unix:!macx:!symbian: LIBS += -L$$PWD/../../../../../home/alex/Programming/lib/ -lUtils

INCLUDEPATH += /mnt/data/Programming/include
DEPENDPATH += /mnt/data/Programming/include

unix:!macx:!symbian: PRE_TARGETDEPS += /mnt/data/Programming/lib/libUtils.a
