#ifndef WIDGETS_GLOBAL_H
#define WIDGETS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WIDGETS_LIBRARY)
#  define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#  define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILS_GLOBAL_H
