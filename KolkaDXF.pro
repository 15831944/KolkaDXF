#-------------------------------------------------
#
# Project created by QtCreator 2012-07-31T21:18:10
#
#-------------------------------------------------

QT       += core gui \
         widgets

TARGET = KolkaDXF
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp \
        cdbfile.cpp \
        rekord.cpp \
    libxls/ole.c \
    libxls/xls.c \
    libxls/xlstool.c

HEADERS  += mainwindow.h \
    cdbfile.h \
    rekord.h \
    libxls/brdb.c.h \
    libxls/brdb.h \
    libxls/ole.h \
    libxls/xls.h \
    libxls/xlsstruct.h \
    libxls/xlstool.h \
    libxls/xlstypes.h

FORMS    += mainwindow.ui

