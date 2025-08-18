#-------------------------------------------------
#
# Project created by QtCreator 2017-06-13T10:50:33
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32|macx {
TARGET = "MOV MetaEdit"
} else {
TARGET = "movmetaedit-gui"
}

win32 {
    LIBS += -luser32
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

    contains(MACSTORE, yes|1) {
        QMAKE_CFLAGS += -gdwarf-2
        QMAKE_CXXFLAGS += -gdwarf-2
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    }
}

TEMPLATE = app

CONFIG += qt release

DEFINES += _LARGE_FILES _FILE_OFFSET_BITS=64

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH      += ../../Source/GUI/Qt
INCLUDEPATH      += ../../Source

SOURCES += \
        ../../Source/GUI/Qt/main.cpp \
        ../../Source/GUI/Qt/core.cpp \
        ../../Source/GUI/Qt/mainwindow.cpp \
        ../../Source/GUI/Qt/tablewidget.cpp \
        ../../Source/GUI/Qt/techtablewidget.cpp \
        ../../Source/GUI/Qt/chantablewidget.cpp \
        ../../Source/GUI/Qt/channeldialog.cpp \
        ../../Source/GUI/Qt/aboutdialog.cpp \
        ../../Source/GUI/Qt/helpdialog.cpp\
        ../../Source/Common/AdID.cpp \
        ../../Source/Common/mp4/mp4_.cpp \
        ../../Source/Common/mp4/mp4_mdat.cpp \
        ../../Source/Common/mp4/mp4_free.cpp \
        ../../Source/Common/mp4/mp4_moov.cpp \
        ../../Source/Common/mp4/mp4_moov_trak.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_tkhd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_tapt.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_tapt_xxxx.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxVideo.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clap.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_colr.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_fiel.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_gama.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_pasp.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_mdcv.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_clli.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxSound.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxx_chan.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_stbl_stsd_xxxxUnknown.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_mdhd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_vmhd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_smhd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_gmhd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_mdia_minf_gmhd_tmcd.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_tref.cpp \
        ../../Source/Common/mp4/mp4_moov_trak_tref_tmcd.cpp \
        ../../Source/Common/mp4/mp4_moov_meta.cpp \
        ../../Source/Common/mp4/mp4_moov_meta_hdlr.cpp \
        ../../Source/Common/mp4/mp4_moov_meta_ilst.cpp \
        ../../Source/Common/mp4/mp4_moov_meta_keys.cpp \
        ../../Source/Common/mp4_Base.cpp \
        ../../Source/Common/mp4_Base_parsing.cpp \
        ../../Source/Common/mp4_Handler.cpp \
        ../../Source/ThirdParty/tfsxml/tfsxml.c \
		../../Source/ZenLib/Conf.cpp \
		../../Source/ZenLib/CriticalSection.cpp \
		../../Source/ZenLib/Dir.cpp \
		../../Source/ZenLib/File.cpp \
		../../Source/ZenLib/FileName.cpp \
		../../Source/ZenLib/InfoMap.cpp \
		../../Source/ZenLib/int128s.cpp \
		../../Source/ZenLib/int128u.cpp \
		../../Source/ZenLib/MemoryDebug.cpp \
		../../Source/ZenLib/OS_Utils.cpp \
		../../Source/ZenLib/PreComp.cpp \
		../../Source/ZenLib/Thread.cpp \
		../../Source/ZenLib/Translation.cpp \
		../../Source/ZenLib/Utils.cpp \
		../../Source/ZenLib/Ztring.cpp \
		../../Source/ZenLib/ZtringList.cpp \
		../../Source/ZenLib/ZtringListList.cpp \
        ../../Source/ZenLib/ZtringListListF.cpp

HEADERS += \
        ../../Source/GUI/Qt/core.h \
        ../../Source/GUI/Qt/mainwindow.h \
        ../../Source/GUI/Qt/tablewidget.h \
        ../../Source/GUI/Qt/techtablewidget.h \
        ../../Source/GUI/Qt/chantablewidget.h \
        ../../Source/GUI/Qt/channeldialog.h \
        ../../Source/GUI/Qt/aboutdialog.h \
        ../../Source/GUI/Qt/helpdialog.h \
        ../../Source/GUI/Qt/config.h \
        ../../Source/Common/AdID.h \
        ../../Source/Common/mp4_Base.h \
        ../../Source/Common/mp4_Handler.h \
        ../../Source/Common/mp4/mp4_.h \
        ../../Source/ThirdParty/tfsxml/tfsxml.h \
        ../../Source/ThirdParty/json/json.hpp \
		../../Source/ZenLib/BitStream.h \
		../../Source/ZenLib/BitStream_Fast.h \
		../../Source/ZenLib/BitStream_LE.h \
		../../Source/ZenLib/Conf.h \
		../../Source/ZenLib/Conf_Internal.h \
		../../Source/ZenLib/CriticalSection.h \
		../../Source/ZenLib/Dir.h \
		../../Source/ZenLib/File.h \
		../../Source/ZenLib/FileName.h \
		../../Source/ZenLib/InfoMap.h \
		../../Source/ZenLib/int128s.h \
		../../Source/ZenLib/int128u.h \
		../../Source/ZenLib/MemoryDebug.h \
		../../Source/ZenLib/MemoryUtils.h \
		../../Source/ZenLib/OS_Utils.h \
		../../Source/ZenLib/PreComp.h \
		../../Source/ZenLib/Thread.h \
		../../Source/ZenLib/Trace.h \
		../../Source/ZenLib/Translation.h \
		../../Source/ZenLib/Utils.h \
		../../Source/ZenLib/Ztring.h \
		../../Source/ZenLib/ZtringList.h \
		../../Source/ZenLib/ZtringListList.h \
        ../../Source/ZenLib/ZtringListListF.h

FORMS += \
        ../../Source/GUI/Qt/mainwindow.ui \
        ../../Source/GUI/Qt/channeldialog.ui \
        ../../Source/GUI/Qt/aboutdialog.ui \
        ../../Source/GUI/Qt/helpdialog.ui

contains(MACSTORE, yes|1) {
    DEFINES           += MACSTORE
    HEADERS           += ../../Source/Common/Mac_Helpers.h
    OBJECTIVE_SOURCES += ../../Source/Common/Mac_Helpers.mm
    LIBS              += -framework Foundation
}

RESOURCES += ../../Source/Resource/Resource.qrc

OBJECTS_DIR      =  _Automated
MOC_DIR          =  _Automated
UI_HEADERS_DIR   =  _Automated
UI_SOURCES_DIR   =  _Automated
UI_DIR           =  _Automated
RCC_DIR          =  _Automated

