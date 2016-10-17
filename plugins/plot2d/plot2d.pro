include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += plot2dplugin.h \
           lineplotwidget.h \
           barplotwidget.h \
           scatterplotwidget.h \
           plotwidget.h \
    pieplotwidget.h \
    stackedplotwidget.h \
    bagplotwidget.h \

SOURCES += plot2dplugin.cpp \
           lineplotwidget.cpp \
           barplotwidget.cpp \
           scatterplotwidget.cpp \
           plotwidget.cpp \
    pieplotwidget.cpp \
    stackedplotwidget.cpp \
    bagplotwidget.cpp \

FORMS += \
    tableplotwidget.ui \
    lineplotsettings.ui \
    barplotsettings.ui \
    scatterplotsettings.ui
