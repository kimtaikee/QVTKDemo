INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(../../lib.pri)

DEFINES += THIRDPARTY_LIBRARY

HEADERS += 3dparty_global.h \
           flowlayout.h \
           QRoundProgressBar.h

SOURCES += flowlayout.cpp \
           QRoundProgressBar.cpp
