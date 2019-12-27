#-------------------------------------------------
#
# Project created by QtCreator 2011-03-18T21:48:45
#
#-------------------------------------------------

QT       += core gui \
            widgets

TARGET = mdpcreator
TEMPLATE = app
CONFIG += warn_on

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui

SOURCES += \
    MDPWindow.cpp \
    main.cpp

HEADERS  += \
    MDPWindow.h

RESOURCES += \
    ressources.qrc

