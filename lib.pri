include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$LIB_NAME

include(demos.pri)

win32 {
    DLLDESTDIR = $$QVTKDEMOS_BINARIES_PATH
}

DESTDIR = $$QVTKDEMOS_LIBRARY_PATH

include(rpath.pri)

TARGET = $$qtLibraryName($$TARGET)

TEMPLATE = lib
CONFIG += shared dll

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

QMAKE_TARGET_DESCRIPTION = "QVTKDemo Library"
