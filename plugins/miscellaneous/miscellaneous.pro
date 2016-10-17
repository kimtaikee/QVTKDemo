include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += miscellaneousplugin.h \
    representationwidget.h \
    backgroundwindow.h \
    colormapwindow.h \
    parametricobjectswindow.h \
    scaleaxeswindow.h

SOURCES += miscellaneousplugin.cpp \
    representationwidget.cpp \
    backgroundwindow.cpp \
    colormapwindow.cpp \
    parametricobjectswindow.cpp \
    scaleaxeswindow.cpp

FORMS += \
    representationwidget.ui \
    backgroundwindow.ui \
    colormapwindow.ui \
    parametricobjectswindow.ui \
    romanconfig.ui \
    randomhillsconfig.ui \
    ellipsoidconfig.ui \
    diniconfig.ui \
    conicspiralconfig.ui \
    splineconfig.ui \
    superellipsoidconfig.ui \
    supertoroidconfig.ui \
    torusconfig.ui \
    scaleaxeswindow.ui


