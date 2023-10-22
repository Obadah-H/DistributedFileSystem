#-------------------------------------------------
#
# Project created by QtCreator 2023-10-5T12:40:10
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = accses_control_decstore
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    dbmanager.cpp \
    nodemanager.cpp

HEADERS += \
    dbmanager.h \
    nodemanager.h
