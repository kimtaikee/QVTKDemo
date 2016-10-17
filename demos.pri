!isEmpty(QVTKDEMOS_PRI_INCLUDED):error("demos.pri already included")

QVTKDEMOS_PRI_INCLUDED = 1

VERSION = 1.0
QMAKE_TARGET_COMPANY = "JasonZhang Inc"
QMAKE_TARGET_PRODUCT = "QVTKDemos"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2016 Jason Zhang"

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += QT_MESSAGELOGCONTEXT

defineReplace(cleanPath) {
    return($$clean_path($$1))
}

defineReplace(targetPath) {
    return($$shell_path($$1))
}
defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

defineReplace(scLibraryName) {
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $${1}_debug$$section(VERSION, ., 0, 0)
          else:win32:RET = $${1}d$$section(VERSION, ., 0, 0)
      }
   }
   isEmpty(RET):RET = $$1$$section(VERSION, ., 0, 0)
   return($$RET)
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

QVTKDEMOS_SOURCE_TREE = $$PWD
isEmpty(QVTKDEMOS_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    QVTKDEMOS_BUILD_TREE = $$cleanPath($$OUT_PWD)
    QVTKDEMOS_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

CONFIG(debug, debug|release) {
    DEBUG_RELEASE = DEBUG
} else {
    DEBUG_RELEASE = RELEASE
}

#CURRENTHOST = WIN32_GCC

win32-msvc2013:contains(QMAKE_HOST.arch, x86): {
    CURRENTHOST = WIN32_VC2013
    QVTKDEMOS_BIN = win32
}

win32-msvc2015:contains(QMAKE_HOST.arch, x86): {
    CURRENTHOST = WIN32_VC2015
    QVTKDEMOS_BIN = win32
}

win32-g++:contains(QMAKE_HOST.arch, x86): {
    CURRENTHOST = WIN32_GCC
    QVTKDEMOS_BIN = win32
}

win32-msvc2013:isEqual(QT_ARCH, x86_64): {
    CURRENTHOST = WIN64_VC2013
    QVTKDEMOS_BIN = win64
}

win32-msvc2015:isEqual(QT_ARCH, x86_64): {
    CURRENTHOST = WIN64_VC2015
    QVTKDEMOS_BIN = win64
}

#win32-g++:isEqual(QT_ARCH, x86_64): {
#    CURRENTHOST = WIN64_GCC
#}

win32-g++:contains(QMAKE_HOST.arch, x86_64): {
    CURRENTHOST = WIN32_GCC
    QVTKDEMOS_BIN = win32
}

unix: isEqual(QT_ARCH, x86_64): {
    CURRENTHOST = LINUX64
    QVTKDEMOS_BIN = linux64
}

QVTKDEMOS_APP_PATH = $$QVTKDEMOS_SOURCE_TREE/build/bin/$${CURRENTHOST}_$$DEBUG_RELEASE
QVTKDEMOS_BINARIES_PATH = $$QVTKDEMOS_APP_PATH/$${QVTKDEMOS_BIN}
QVTKDEMOS_LIBRARY_PATH = $$QVTKDEMOS_SOURCE_TREE/build/lib/$${CURRENTHOST}_$$DEBUG_RELEASE
QVTKDEMOS_PLUGIN_PATH  = $$QVTKDEMOS_APP_PATH/$${QVTKDEMOS_BIN}/plugins

#拷贝一个或多个文件到指定目录, 第一个参数是待拷贝文件列表, 第二个参数是指定目录
defineReplace(copyFile) {
    unset(files)
    unset(dist)
    files = $$1 #文件列表
    #dist = $$targetPath($$2) #指定目录
    dist = $$2
    win32 {
        COPY = copy /y
        MKDIR = mkdir
    } else {
        COPY = cp
        MKDIR = mkdir -p
    }

    dist = $$system_path($$dist)
    !exists($$dist):system($$MKDIR $$dist)
    for (f, files) {
        #dist_file = $$dist/$$basename(f)
        #强制覆盖已存在的 todo linux下需测试
        #!exists($$dist_file): {
        f = $$system_path($$f)
        system($$COPY $$f "$$dist")
        #}
    }
    return($$files)
}

#拷贝一个目录的所有内容到另一个目录中, 第一个参数是待拷贝目录, 第二个参数是指定目录
defineReplace(copyFolder) {
    unset(sourceDir)
    unset(dist)
    sourceDir = $$system_path($$1)
    dist = $$system_path($$2)
    win32 {
        COPY = xcopy /e /y
        MKDIR = mkdir
    } else {
        COPY = cp -rf
        MKDIR = mkdir -p
    }

    #强制覆盖已存在的 todo linux下需测试
    #!exists($$dist) {
        system($$MKDIR $$dist)
        system($$COPY $$sourceDir $$dist)
    #}
    return($$dist)
}

INCLUDEPATH *= \
    $$QVTKDEMOS_SOURCE_TREE/libs $$QVTKDEMOS_SOURCE_TREE/shared

LIBS *= -L$$QVTKDEMOS_LIBRARY_PATH

!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

win32-msvc* {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #编译出可在xp下运行的版本
    QMAKE_SUBSYSTEM_SUFFIX = ,5.01
}

contains(QT, core): QT += concurrent
contains(QT, gui): QT += widgets

THIRDPARTYINCLUDE = $$PWD/include/3rdparty
INCLUDEPATH *= $$THIRDPARTYINCLUDE

CONFIG += c++11

# 递归决议库依赖性
done_libs =
for(ever) {
    isEmpty(LIB_DEPENDS): \
        break()
    done_libs += $$LIB_DEPENDS
    for(dep, LIB_DEPENDS) {
        include($$PWD/libs/$$dep/$${dep}_dependencies.pri) # this depends on your project structure
        LIBS += -l$$scLibraryName($$LIB_NAME)
    }
    LIB_DEPENDS = $$unique(LIB_DEPENDS)
    LIB_DEPENDS -= $$unique(done_libs)
}
