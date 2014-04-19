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
    globals.cpp

HEADERS  += radiocontrol.h \
    globals.h

FORMS    += radiocontrol.ui

unix:!macx:!symbian: LIBS += -L$$PWD/../../../../../home/alex/Programming/lib/ -lUtils

INCLUDEPATH += $$PWD/../../../../../home/alex/Programming/include
DEPENDPATH += $$PWD/../../../../../home/alex/Programming/include

unix:!macx:!symbian: PRE_TARGETDEPS += $$PWD/../../../../../home/alex/Programming/lib/libUtils.a
