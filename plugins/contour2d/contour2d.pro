include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += contour2dplugin.h \
    contour2dinteractorstyle.h \
    contourwindow.h \
    smoothcontourgenerator.h \
    bandedcontourgenerator.h \
    abstractcontourgenerator.h

SOURCES += contour2dplugin.cpp \
    contour2dinteractorstyle.cpp \
    contourwindow.cpp \
    smoothcontourgenerator.cpp \
    bandedcontourgenerator.cpp \
    abstractcontourgenerator.cpp

FORMS += \
    contourwindow.ui

RESOURCES += \
    data.qrc



