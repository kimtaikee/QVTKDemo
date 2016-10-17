INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(../../lib.pri)
include(../../vtk_inc.pri)

DEFINES += VTKUTILS_LIBRARY

HEADERS +=  vtkutils_global.h \
            vtkwidget.h \
            vtkutils.h \
            vtkplotwidget.h \
            filereaderutils.h \
            vtkUnstructuredGridRelevantPointsFilter.h \
            surface.h \
            contour.h \
            abstractimageprocessor.h \
            rendererslayoutalgo.h \
            modeltovtktableconverter.h \
            vtkplot2dwidget.h \
            vtkplot3dwidget.h \
            datafilter.h \
            linewidgetobserver.h \
            abstractwidgetobserver.h \
            spherewidgetobserver.h \
            implicitplanewidgetobserver.h \
            pointwidgetobserver.h \
            moveactorinteractorstyle.h \
            movepointinteractorstyle.h \
            boxwidgetobserver.h \
            pointstopolydataconverter.h \
            actorexporter.h \
            distancewidgetobserver.h \
            anglewidgetobserver.h \
            sliderwidgetobserver.h \
            contourwidgetobserver.h \
            vtkplotpiewidget.h

SOURCES += \
            vtkwidget.cpp \
            vtkutils.cpp \
            vtkplotwidget.cpp \
            filereaderutils.cpp \
            vtkUnstructuredGridRelevantPointsFilter.cpp \
            surface.cpp \
            contour.cpp \
            abstractimageprocessor.cpp \
            modeltovtktableconverter.cpp \
            vtkplot2dwidget.cpp \
            vtkplot3dwidget.cpp \
            datafilter.cpp \
            linewidgetobserver.cpp \
            spherewidgetobserver.cpp \
            abstractwidgetobserver.cpp \
            implicitplanewidgetobserver.cpp \
            pointwidgetobserver.cpp \
            moveactorinteractorstyle.cpp \
            movepointinteractorstyle.cpp \
            boxwidgetobserver.cpp \
            pointstopolydataconverter.cpp \
            actorexporter.cpp \
            distancewidgetobserver.cpp \
            anglewidgetobserver.cpp \
            sliderwidgetobserver.cpp \
            contourwidgetobserver.cpp \
            vtkplotpiewidget.cpp
