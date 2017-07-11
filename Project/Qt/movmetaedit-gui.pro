#-------------------------------------------------
#
# Project created by QtCreator 2017-06-13T10:50:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32|macx {
TARGET = "MOV MetaEdit"
} else {
TARGET = "movmetaedit-gui"
}

win32 {
    RC_FILE = movmetaedit-gui.rc
    contains(QT_ARCH, i386) {
        DESTDIR = Win32
    }
    contains(QT_ARCH, x86_64) {
        DESTDIR = x64
    }
}

macx {
    LIBS += -framework CoreFoundation
    QMAKE_INFO_PLIST = ../Mac/Info.plist
    ICON = ../../Source/Resource/Image/Icon.icns
}

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

