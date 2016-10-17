include($$replace(_PRO_FILE_PWD_, ([^/]+$), \\1/\\1_dependencies.pri))
TARGET = $$PLUGIN_NAME

include(demos.pri)

isEmpty(PROVIDER) {
    PROVIDER = qvtkdemos
} else {
    LIBS += -L$$PLUGIN_PATH/qvtkdemos
}
# DESTDIR = $$QVTKDEMOS_PLUGIN_PATH/$$PROVIDER
DESTDIR = $$QVTKDEMOS_PLUGIN_PATH # we want the plugins be put under the "plugins" directory
contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryName($$TARGET)

QMAKE_TARGET_DESCRIPTION = "QVTK Demos Plugin"
