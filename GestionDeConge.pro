#-------------------------------------------------
#
# Project created by QtCreator 2016-01-15T23:20:46
#
#-------------------------------------------------

QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GestionDeConge
TEMPLATE = app


SOURCES += main.cpp\
    FenResponsable.cpp \
    FenConnexion.cpp \
    FenEmploye.cpp \
    WindowCenter.cpp \
    Fenrh.cpp

HEADERS  += \
    FenResponsable.h \
    FenEmploye.h \
    FenConnexion.h \
    WindowCenter.h \
    connectionBdd.h \
    Fenrh.h

FORMS    += \
    fenConnexion.ui \
    fenEmploye.ui \
    FenResponsable.ui \
    Fenrh.ui
