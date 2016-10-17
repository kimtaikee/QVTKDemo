include(../../plugin.pri)
include(../../vtk_inc.pri)

HEADERS += filereaderplugin.h \
           widget.h \
    mainwidget.h \
    filetree.h

SOURCES += filereaderplugin.cpp \
           widget.cpp \
    mainwidget.cpp \
    filetree.cpp

FORMS += \
    mainwidget.ui
