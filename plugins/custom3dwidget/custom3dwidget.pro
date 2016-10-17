include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += custom3dwidgetplugin.h \
    customxyplotwidget.h \
    customxyplotrepresentation.h \
    usecustomwidgetwindow.h \
    vtkarrowwidget.h \
    usearrowwidgetwindow.h \
    vtkarrowrepresentation.h

SOURCES += custom3dwidgetplugin.cpp \
    customxyplotwidget.cpp \
    customxyplotrepresentation.cpp \
    usecustomwidgetwindow.cpp \
    vtkarrowwidget.cpp \
    usearrowwidgetwindow.cpp \
    vtkarrowrepresentation.cpp

FORMS += \
    usecustomwidgetwindow.ui \
    usearrowwidgetwindow.ui


