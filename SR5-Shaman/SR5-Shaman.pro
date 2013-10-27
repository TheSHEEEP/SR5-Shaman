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
    ui/settings/preferenceswindow.cpp \
    ui/settings/applayoutsettings.cpp \
    ui/settings/currentsettingscontainer.cpp \
    data/appstatus.cpp \
    ui/character/creationinitializingwindow.cpp \
    ui/character/characterpreviewsideinfo.cpp \
    ui/character/creationsideinfo.cpp \
    ui/character/tabs/chareditmisctab.cpp

HEADERS  += mainwindow.h \
    ui/mainsplitview.h \
    ui/settings/preferenceswindow.h \
    ui/settings/applayoutsettings.h \
    ui/settings/currentsettingscontainer.h \
    data/appstatus.h \
    ui/character/creationinitializingwindow.h \
    ui/character/characterpreviewsideinfo.h \
    ui/character/creationsideinfo.h \
    ui/character/tabs/chareditmisctab.h

FORMS    += mainwindow.ui \
    ui/mainsplitview.ui \
    ui/settings/preferenceswindow.ui \
    ui/settings/applayoutsettings.ui \
    ui/settings/currentsettingscontainer.ui \
    ui/character/creationinitializingwindow.ui \
    ui/character/characterpreviewsideinfo.ui \
    ui/character/creationsideinfo.ui \
    ui/character/tabs/chareditmisctab.ui

TRANSLATIONS = sr5shaman_de.ts

OTHER_FILES += \
    sr5shaman_de.ts

RESOURCES += \
    resources/resources.qrc
