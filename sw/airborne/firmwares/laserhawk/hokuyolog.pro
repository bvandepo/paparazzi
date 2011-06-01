# -------------------------------------------------
# Project created by QtCreator 2010-06-23T10:59:18
# -------------------------------------------------
QT -= gui

# HOMEREP = /home/guillaume
HOMEREP = /Users/bvandepo
TARGET = hokuyogood
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    chokuyoplus.cpp \
    pnm_io2.cpp \
    RLE.cpp
INCLUDEPATH += $$HOMEREP/openrobots/include \
    $$HOMEREP/openrobots/include/gearbox/
LIBS += -L/opt/local/lib/

# -ljpeg
LIBS += -L$$HOMEREP/openrobots/lib/gearbox \
    -lhokuyo_aist

# LIBS += -ljpeg
HEADERS += chokuyoplus.h \
    pnm_io2.h \
    std.h \
    $$HOMEREP/openrobots/include/gearbox/hokuyo_aist/hokuyo_aist.h \
    RLE.h
