#-------------------------------------------------
#
# Project created by QtCreator 2013-10-21T13:23:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SR5-Shaman
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ui/mainsplitview.cpp \
    ui/characterpreview.cpp \
    ui/settings/preferenceswindow.cpp \
    ui/settings/applayoutsettings.cpp \
    ui/settings/currentsettingscontainer.cpp

HEADERS  += mainwindow.h \
    ui/mainsplitview.h \
    ui/characterpreview.h \
    ui/settings/preferenceswindow.h \
    ui/settings/applayoutsettings.h \
    ui/settings/currentsettingscontainer.h

FORMS    += mainwindow.ui \
    ui/mainsplitview.ui \
    ui/characterpreview.ui \
    ui/settings/preferenceswindow.ui \
    ui/settings/applayoutsettings.ui \
    ui/settings/currentsettingscontainer.ui

TRANSLATIONS = sr5shaman_de.ts

OTHER_FILES += \
    sr5shaman_de.ts

RESOURCES += \
    resources/resources.qrc
