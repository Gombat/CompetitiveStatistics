#-------------------------------------------------
#
# Project created by QtCreator 2016-04-21T01:14:39
#
#-------------------------------------------------

#CONFIG     += debug

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CompetetiveStatistics
TEMPLATE = app

QTPLUGIN += QSQLMYSQL

SOURCES += main.cpp\
        mainwindow.cpp \
    database.cpp \
    challongeimporter.cpp \
    sessioneditwizard.cpp \
    sessioneditpagesession.cpp \
    sessioneditpageplayers.cpp \
    sessioneditpagematches.cpp \
    sessioneditpageoverview.cpp \
    data.cpp

HEADERS  += mainwindow.h \
    database.h \
    challongeimporter.h \
    sessioneditwizard.h \
    sessioneditpagesession.h \
    sessioneditpageplayers.h \
    sessioneditpagematches.h \
    sessioneditpageoverview.h \
    data.h

FORMS    += mainwindow.ui

RESOURCES     = ./CompetitiveStatistics.qrc

