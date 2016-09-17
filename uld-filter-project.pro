#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T16:41:12
#
#-------------------------------------------------

QT       += multimedia quick core widgets

QT       -= gui

TARGET = uld-filter-project
TEMPLATE = lib

DEFINES += ULDFILTERPROJECT_LIBRARY

SOURCES += uldfilterproject.cpp \
    uldsocket.cpp \
    uldhelper.cpp

HEADERS += uldfilterproject.h\
        uld-filter-project_global.h \
    uldsocket.h \
    qobject__.h \
    uldhelper.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
