#-------------------------------------------------
#
# Project created by QtCreator 2013-01-13T12:23:48
#
#-------------------------------------------------

QT       += core
QT       -= gui
QT       += network

CONFIG   += console
CONFIG   -= app_bundle

TARGET = PresidentReseau

TEMPLATE = app


SOURCES += main.cpp \
    channel.cpp \
    application_server.cpp \
    client.cpp

HEADERS  += \
    channel.hpp \
    application_server.hpp \
    client.hpp

#FORMS    +=


