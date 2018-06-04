#-------------------------------------------------
#
# Project created by QtCreator 2018-05-26T08:01:48
#
#-------------------------------------------------

QT       += core gui network networkauth sql multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Spotifinder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    qtspotify.cpp \
    urldatadownloader.cpp \
    spotifyuserinfo.cpp \
    utils.cpp \
    databasecontroler.cpp \
    labelitem.cpp \
    modelspotifysearch.cpp

HEADERS  += \
    clientid.h \
    qtspotify.h \
    urldatadownloader.h \
    spotifyuserinfo.h \
    utils.h \
    databasecontroler.h \
    labelitem.h \
    modelspotifysearch.h

FORMS    += \
    labelitem.ui \
    qtspotify.ui

DISTFILES +=

RESOURCES += \
    images.qrc

