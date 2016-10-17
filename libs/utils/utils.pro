INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(../../lib.pri)

DEFINES += UTILS_LIBRARY

HEADERS +=  utils_global.h \
            doublespinboxdelegate.h\
            tablemodel.h \
            utils.h \
            iplugin.h \
            ivtkplugin.h \
            pointsreader.h \
            pointstomodelconverter.h \
            signalledrunable.h \
            modeltopointsconverter.h \
            modeltovectorsconverter.h \
            signalblocker.h \
            colorseries.h \
            point3f.h \
            vector4f.h \

SOURCES +=  doublespinboxdelegate.cpp \
            tablemodel.cpp \
            ivtkplugin.cpp \
            utils.cpp \
            pointsreader.cpp \
            pointstomodelconverter.cpp \
            signalledrunable.cpp \
            modeltopointsconverter.cpp \
            modeltovectorsconverter.cpp \
            signalblocker.cpp \
            colorseries.cpp \
            point3f.cpp \
            vector4f.cpp \


