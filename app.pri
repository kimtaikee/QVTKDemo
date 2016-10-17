include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$APP_NAME

include(demos.pri)
include(rpath.pri)
# 预编译头文件
#isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$PWD/shared/simcube_gui_pch.h

DESTDIR = $$QVTKDEMOS_BINARIES_PATH
TARGET = $$qtLibraryName($$TARGET)
TEMPLATE = app
