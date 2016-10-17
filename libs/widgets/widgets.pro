INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(../../lib.pri)

DEFINES += WIDGETS_LIBRARY

HEADERS += \ 
    imagecompareboard.h \
    widgets_global.h \
    colorcombobox.h \
    colorpushbutton.h \
    fontpushbutton.h \
    penstylebox.h \
    qtcolorpicker.h \
    tagwidget.h \
    gradientcombobox.h

SOURCES += \ 
    imagecompareboard.cpp \
    colorcombobox.cpp \
    colorpushbutton.cpp \
    fontpushbutton.cpp \
    penstylebox.cpp \
    qtcolorpicker.cpp \
    tagwidget.cpp \
    gradientcombobox.cpp


