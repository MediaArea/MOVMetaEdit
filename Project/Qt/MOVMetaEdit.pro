#-------------------------------------------------
#
# Project created by QtCreator 2017-06-13T10:50:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = movmetaedit-gui
macx:TARGET=MOVMetaEdit
win32:TARGET=MOV_MetaEdit

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH      += ../../Source/GUI/Qt

SOURCES += \
        ../../Source/GUI/Qt/main.cpp \
        ../../Source/GUI/Qt/core.cpp \
        ../../Source/GUI/Qt/mainwindow.cpp \
        ../../Source/GUI/Qt/tablewidget.cpp \
        ../../Source/GUI/Qt/aboutdialog.cpp \
        ../../Source/GUI/Qt/helpdialog.cpp

HEADERS += \
        ../../Source/GUI/Qt/core.h \
        ../../Source/GUI/Qt/mainwindow.h \
        ../../Source/GUI/Qt/tablewidget.h \
        ../../Source/GUI/Qt/aboutdialog.h \
        ../../Source/GUI/Qt/helpdialog.h \
        ../../Source/GUI/Qt/config.h \

FORMS += \
        ../../Source/GUI/Qt/mainwindow.ui \
        ../../Source/GUI/Qt/aboutdialog.ui \
        ../../Source/GUI/Qt/helpdialog.ui

RESOURCES += ../../Source/Resource/Resource.qrc

macx:QMAKE_INFO_PLIST = ../Mac/Info.plist
macx:ICON = ../../Source/Resource/Image/Icon.icns
win32:RC_FILE = MOVMetaEdit.rc
