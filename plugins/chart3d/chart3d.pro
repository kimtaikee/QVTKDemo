include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += chart3dplugin.h \
    chart3dwindow.h \
    points3dwindow.h \
    lines3dwindow.h \
    bar3dwindow.h

SOURCES += chart3dplugin.cpp \
    chart3dwindow.cpp \
    points3dwindow.cpp \
    lines3dwindow.cpp \
    bar3dwindow.cpp

FORMS += \
    chart3dwindow.ui \
    configpoints3d.ui


