include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += pickingplugin.h \
    pickactorwindow.h \
    drawspherewindow.h \
    drawsurfacewindow.h \
    mouseeventinteractorstyle.h \
    pickcellwindow.h \
    pickcellinteractorstyle.h \
    selectareainteractorstyle.h \
#    selectareawindow.h

SOURCES += pickingplugin.cpp \
    pickactorwindow.cpp \
    drawspherewindow.cpp \
    drawsurfacewindow.cpp \
    mouseeventinteractorstyle.cpp \
    pickcellwindow.cpp \
    pickcellinteractorstyle.cpp \
    selectareainteractorstyle.cpp \
#    selectareawindow.cpp

FORMS += \
    pickactorwindow.ui \
    drawspherewindow.ui \
    drawsurfacewindow.ui \
    pickcellwindow.ui \
    selectareawindow.ui


