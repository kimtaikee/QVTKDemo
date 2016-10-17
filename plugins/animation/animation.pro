include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += animationplugin.h \
    basicanimationwindow.h \
    animationscenewindow.h

SOURCES += animationplugin.cpp \
    basicanimationwindow.cpp \
    animationscenewindow.cpp

FORMS += \
    basicanimationwindow.ui \
    animationscenewindow.ui

RESOURCES += \
    res.qrc


