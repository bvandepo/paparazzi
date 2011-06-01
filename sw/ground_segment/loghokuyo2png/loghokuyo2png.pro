# -------------------------------------------------
# Project created by QtCreator 2010-06-23T10:59:18
# -------------------------------------------------
QT -= gui

 
HOMEREP = /Users/bvandepo
TARGET = loghokuyo2png
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += loghokuyo2png.cpp \
        pnm_io2.cpp \
        ../../airborne/firmwares/laserhawk/RLE.cpp

#INCLUDEPATH += $$HOMEREP/openrobots/include \
#     $$HOMEREP/openrobots/include/gearbox/



LIBS += -L/opt/local/lib/
#-ljpeg


#LIBS += -L$$HOMEREP/openrobots/lib/gearbox \
#    -lhokuyo_aist



#LIBS += -ljpeg


HEADERS += pnm_io2.h \
    ../../airborne/firmwares/laserhawk/RLE.h
 #    $$HOMEREP/openrobots/include/gearbox/hokuyo_aist/hokuyo_aist.h

