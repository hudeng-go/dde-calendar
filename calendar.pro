######################################################################
# Automatically generated by qmake (3.1) Tue Aug 6 23:56:24 2019
######################################################################
QT += core gui widgets sql svg dbus

TEMPLATE = app
TARGET = dde-calendar
INCLUDEPATH += .
CONFIG += c++11 link_pkgconfig
QT += dtkwidget dtkgui
PKGCONFIG += dframeworkdbus
# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/calendardbus.h \
           src/calendarview.h \
           src/calendarwindow.h \
           src/constants.h \
           src/dbuscalendar_adaptor.h \
           src/infoview.h \
           src/spinner.h \
           src/weekindicator.h \
           src/environments.h \
           src/yearview.h \
           src/yearwindow.h \
    src/calendarmainwindow.h \
    src/monthview.h \
    src/monthweekview.h \
    src/monthwindow.h \
    src/monthdayview.h \
    src/schedulecoormanage.h \
    src/dbmanager.h \
    src/schedulestructs.h \
    src/scheduleitem.h \
    src/graphicsview.h \
    src/scheduleview.h \
    src/schceduledayview.h \
    src/weekheadview.h \
    src/weekview.h \
    src/weekwindow.h \
    src/daymonthview.h \
    src/schcedulesearchview.h \
    src/dayhuangliview.h \
    src/daywindow.h \
    src/verticalscroll.h \
    src/calendartimeeidt.h \
    src/verticalscrolltext.h \
    src/schceduledlg.h \
    src/myschceduleview.h \
    src/cayearmonthedit.h \
    src/timeedit.h \
    src/timeverticalscroll.h \
    src/customcalendarwidget.h \
    src/schedulesdbus.h \
    src/scheduledatamanage.h \
    src/schcedulealldayview.h \
    src/creatorparschedule.h \
    src/alldayschceduleview.h \
    src/monthschceduleview.h \
    src/schcedulectrldlg.h \
    src/timeeditctrl.h \
    src/alldayschceduleweekview.h \
    src/exportedinterface.h \
    src/configsettings.h \
    src/singleton.h \
    src/shortcut.h \
    src/todybutton.h
SOURCES += src/calendardbus.cpp \
           src/calendarview.cpp \
           src/calendarwindow.cpp \
           src/dbuscalendar_adaptor.cpp \
           src/infoview.cpp \
           src/main.cpp \
           src/spinner.cpp \
           src/weekindicator.cpp \
           src/yearview.cpp \
           src/yearwindow.cpp \
    src/calendarmainwindow.cpp \
    src/monthview.cpp \
    src/monthweekview.cpp \
    src/monthwindow.cpp \
    src/monthdayview.cpp \
    src/dbmanager.cpp \
    src/scheduleitem.cpp \
    src/graphicsview.cpp \
    src/schedulecoormanage.cpp \
    src/scheduleview.cpp \
    src/schceduledlg.cpp \
    src/schceduledayview.cpp \
    src/weekheadview.cpp \
    src/weekview.cpp \
    src/weekwindow.cpp \
    src/daymonthview.cpp \
    src/schcedulesearchview.cpp \
    src/dayhuangliview.cpp \
    src/daywindow.cpp \
    src/verticalscrolltext.cpp \
    src/calendartimeeidt.cpp \
    src/verticalscroll.cpp \
    src/myschceduleview.cpp \
    src/timeverticalscroll.cpp \
    src/cayearmonthedit.cpp \
    src/customcalendarwidget.cpp \
    src/timeedit.cpp \
    src/schedulesdbus.cpp \
    src/scheduledatamanage.cpp \
    src/schcedulealldayview.cpp \
    src/creatorparschedule.cpp \
    src/alldayschceduleview.cpp \
    src/monthschceduleview.cpp \
    src/schcedulectrldlg.cpp \
    src/timeeditctrl.cpp \
    src/alldayschceduleweekview.cpp \
    src/exportedinterface.cpp \
    src/configsettings.cpp \
    src/shortcut.cpp \
    src/todybutton.cpp
RESOURCES += src/resources.qrc
#TRANSLATIONS += translations/dde-calendar.ts \
#                translations/desktop/desktop_zh_CN.ts\
#                translations/dde-calendar_zh_CN.ts
isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/dde-calendar

# Automating generation .qm files from .ts files
!system($$PWD/translate_generation.sh): error("Failed to generate translation")

target.path = $$INSTROOT$$BINDIR
icon_files.path = $$PREFIX/share/icons/hicolor/scalable/apps/
icon_files.files = $$PWD/images/dde-calendar.svg

desktop.path = $$INSTROOT$$APPDIR
desktop.files = dde-calendar.desktop

manual.path = /usr/share/dman/
manual.files = $$PWD/dman/*

translations.path = $$INSTROOT$$DSRDIR/translations
translations.files = translations/*.qm

dbus_service.files = $$PWD/com.deepin.Calendar.service
dbus_service.path = $$PREFIX/share/dbus-1/services

INSTALLS += target desktop icon_files manual dbus_service

isEmpty(TRANSLATIONS) {
     include(translations.pri)
}

TRANSLATIONS_COMPILED = $$TRANSLATIONS
TRANSLATIONS_COMPILED ~= s/\.ts/.qm/g

translations.files = $$TRANSLATIONS_COMPILED
INSTALLS += translations
CONFIG *= update_translations release_translations

CONFIG(update_translations) {
    isEmpty(lupdate):lupdate=lupdate
    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
}
CONFIG(release_translations) {
    isEmpty(lrelease):lrelease=lrelease
    system($$lrelease $$_PRO_FILE_)
}

DSR_LANG_PATH += $$DSRDIR/translations
DEFINES += "DSR_LANG_PATH=\\\"$$DSR_LANG_PATH\\\""

#DISTFILES += \
#    image/newUI/focus/close-focus.svg

DISTFILES += \
    translations.pri
