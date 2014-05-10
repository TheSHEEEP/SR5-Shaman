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
    ui/character/tabs/chareditmisctab.cpp \
    rules/rules.cpp \
    rules/metatyperules.cpp \
    ui/character/tabs/chareditmagictab.cpp \
    ui/character/tabs/chareditattributetab.cpp \
    rules/attributerules.cpp \
    data/character/characterchoices.cpp \
    data/character/charactervalues.cpp \
    rules/magicrules.cpp \
    ui/utils/priorityeventfilter.cpp \
    ui/models/skilltreemodel.cpp \
    rules/skillrules.cpp \
    ui/models/skillsortfilterproxymodel.cpp \
    ui/models/skilldelegate.cpp \
    ui/models/magictreemodel.cpp \
    ui/models/magicdelegate.cpp \
    ui/models/magicsortfilterproxymodel.cpp \
    ui/character/popups/customdescriptorpopup.cpp \
    rules/misc/customchoice.cpp \
    data/dictionary.cpp \
    data/character/effectregistry.cpp \
    rules/effects/effect.cpp \
    rules/effects/condition.cpp \
    ui/character/tabs/chareditqualitytab.cpp \
    ui/models/qualitytreemodel.cpp \
    ui/models/qualitydelegate.cpp \
    ui/models/qualitysortfilterproxymodel.cpp \
    rules/qualityrules.cpp \
    ui/character/tabs/chareditskilltab.cpp \
    globaleventfilter.cpp \
    ui/character/popups/specializationdialog.cpp

HEADERS  += mainwindow.h \
    ui/mainsplitview.h \
    ui/settings/preferenceswindow.h \
    ui/settings/applayoutsettings.h \
    ui/settings/currentsettingscontainer.h \
    data/appstatus.h \
    ui/character/creationinitializingwindow.h \
    ui/character/characterpreviewsideinfo.h \
    ui/character/creationsideinfo.h \
    ui/character/tabs/chareditmisctab.h \
    rules/rules.h \
    rules/metatyperules.h \
    ui/character/tabs/chareditmagictab.h \
    ui/character/tabs/chareditattributetab.h \
    rules/attributerules.h \
    data/character/characterchoices.h \
    data/character/charactervalues.h \
    rules/magicrules.h \
    ui/utils/priorityeventfilter.h \
    ui/models/skilltreemodel.h \
    rules/skillrules.h \
    ui/models/skillsortfilterproxymodel.h \
    ui/models/skilldelegate.h \
    ui/models/magictreemodel.h \
    ui/models/magicdelegate.h \
    ui/models/magicsortfilterproxymodel.h \
    ui/character/popups/customdescriptorpopup.h \
    rules/misc/customchoice.h \
    data/dictionary.h \
    data/character/effectregistry.h \
    rules/effects/effect.h \
    rules/effects/condition.h \
    ui/character/tabs/chareditqualitytab.h \
    ui/models/qualitytreemodel.h \
    ui/models/qualitydelegate.h \
    ui/models/qualitysortfilterproxymodel.h \
    rules/qualityrules.h \
    rules/commonrules.h \
    ui/character/tabs/chareditskilltab.h \
    globaleventfilter.h \
    ui/character/popups/specializationdialog.h

FORMS    += mainwindow.ui \
    ui/mainsplitview.ui \
    ui/settings/preferenceswindow.ui \
    ui/settings/applayoutsettings.ui \
    ui/settings/currentsettingscontainer.ui \
    ui/character/creationinitializingwindow.ui \
    ui/character/characterpreviewsideinfo.ui \
    ui/character/creationsideinfo.ui \
    ui/character/tabs/chareditmisctab.ui \
    ui/character/tabs/chareditmagictab.ui \
    ui/character/tabs/chareditattributetab.ui \
    ui/character/popups/customdescriptorpopup.ui \
    ui/character/tabs/chareditqualitytab.ui \
    ui/character/tabs/chareditskilltab.ui \
    ui/character/popups/specializationdialog.ui

TRANSLATIONS = sr5shaman_de.ts

OTHER_FILES += \
    sr5shaman_de.ts

RESOURCES += \
    resources/resources.qrc \
    stylesheets/qdarkstyle/style.qrc \
    stylesheets/darkOrange/darkOrange.qrc \
    stylesheets/dark/dark.qrc

QMAKE_CXXFLAGS += -Wno-ignored-qualifiers -Wno-unused-parameter -std=gnu++11
