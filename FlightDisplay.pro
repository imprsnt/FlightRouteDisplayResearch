#-------------------------------------------------
#
# Project created by QtCreator 2014-04-20T13:05:11
#
#-------------------------------------------------

QT       += core gui xml opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlightDisplay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    CustomWidgets.cpp \
    UtilityClasses.cpp \
    selectionwindow.cpp \
    displaygraphicv1.cpp \
    displaygraphicv2.cpp \
    displaygraphicv3.cpp \
    displaytextv1.cpp \
    CustomTextDisplayWidgets.cpp \
    displaytextv2.cpp \
    displaytextv3.cpp \
    CustomVisualDisplayWidgets.cpp \
    scrollabletextdisplay1.cpp \
    scrollabletextdisplay2.cpp \
    scrollabletextdisplay3.cpp \
    selectionwidowforscrollabledisplays.cpp \
    CustomScrollableTextDisplayWidgets.cpp \
    scrollablegraphicdisplay1.cpp \
    scrollablegraphicdisplay2.cpp \
    scrollablegraphicdisplay3.cpp \
    CustomScrollableGraphicsDisplayWidgets.cpp

HEADERS  += mainwindow.h \
    CustomWidgets.h \
    UtilityClasses.h \
    selectionwindow.h \
    displaygraphicv1.h \
    displaygraphicv2.h \
    displaygraphicv3.h \
    CustomTextDisplayWidgets.h \
    displaytextv1.h \
    displaytextv2.h \
    displaytextv3.h \
    CustomVisualDisplayWidgets.h \
    scrollabletextdisplay1.h \
    scrollabletextdisplay2.h \
    scrollabletextdisplay3.h \
    CustomScrollableTextDisplayWidgets.h \
    selectionwidowforscrollabledisplays.h \
    scrollablegraphicdisplay1.h \
    scrollablegraphicdisplay2.h \
    scrollablegraphicdisplay3.h \
    CustomScrollableGraphicsDisplayWidgets.h

FORMS    += mainwindow.ui \
    selectionwindow.ui \
    displaygraphicv1.ui \
    displaygraphicv2.ui \
    displaygraphicv3.ui \
    displaytextv1.ui \
    displaytextv2.ui \
    displaytextv3.ui \
    scrollabletextdisplay1.ui \
    scrollabletextdisplay2.ui \
    scrollabletextdisplay3.ui \
    selectionwidowforscrollabledisplays.ui \
    scrollablegraphicdisplay1.ui \
    scrollablegraphicdisplay2.ui \
    scrollablegraphicdisplay3.ui

LIBS += -lglut

RC_FILE += FlightDisplay.rc

RESOURCES += imageresource.qrc

OTHER_FILES += \
    ../../../../../../Users/RMPLplus/Desktop/RACHEL APP/m.PNG \
    ../../../../../../Users/RMPLplus/Desktop/RACHEL APP/cx.PNG
