include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += buildinwidgetsplugin.h \
    basewidgetwindow.h \
    distancewidgetwindow.h \
    anglewidgetwindow.h \
    sliderwidgetwindow.h \
    textwidgetwindow.h \
    scalarbarwidgetwindow.h \
    contourwidgetwindow.h \
    camerawidgetwindow.h

SOURCES += buildinwidgetsplugin.cpp \
    basewidgetwindow.cpp \
    distancewidgetwindow.cpp \
    anglewidgetwindow.cpp \
    sliderwidgetwindow.cpp \
    textwidgetwindow.cpp \
    scalarbarwidgetwindow.cpp \
    contourwidgetwindow.cpp \
    camerawidgetwindow.cpp


FORMS += \
    basewidgetwindow.ui \
    distancewidgetconfig.ui \
    anglewidgetconfig.ui \
    sliderwidgetconfig.ui \
    textwidgetconfig.ui \
    scalarbarwidgetconfig.ui \
    contourwidgetconfig.ui \
    camerawidgetconfig.ui


