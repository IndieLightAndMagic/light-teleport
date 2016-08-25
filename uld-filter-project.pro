#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T16:41:12
#
#-------------------------------------------------

QT       += multimedia quick core

QT       -= gui

TARGET = uld-filter-project
TEMPLATE = lib

DEFINES += ULDFILTERPROJECT_LIBRARY

SOURCES += uldfilterproject.cpp

HEADERS += uldfilterproject.h\
        uld-filter-project_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
