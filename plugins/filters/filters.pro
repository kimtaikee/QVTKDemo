include(../../vtk_inc.pri)
include(../../plugin.pri)

HEADERS += filtersplugin.h \
            isosurfacewindow.h \
            clipwindow.h \
            filterwindow.h \
            thresholdwindow.h \
            cutwindow.h \
            slicewindow.h \
            streamlinewindow.h \
            glyphwindow.h \
            probewindow.h \
            qcustomplot.h \
            decimatewindow.h \
            smoothwindow.h \

SOURCES += filtersplugin.cpp \
            isosurfacewindow.cpp \
            clipwindow.cpp \
            filterwindow.cpp \
            thresholdwindow.cpp \
            cutwindow.cpp \
            slicewindow.cpp \
            streamlinewindow.cpp \
            glyphwindow.cpp \
            probewindow.cpp \
            qcustomplot.cpp \
            decimatewindow.cpp \
            smoothwindow.cpp \

FORMS += \
    generalfilterwindow.ui \
    isosurfaceconfig.ui \
    cutconfig.ui \
    glyphconfig.ui \
    streamlineconfig.ui \
    probeconfig.ui \
    decimateconfig.ui \
    smoothconfig.ui


