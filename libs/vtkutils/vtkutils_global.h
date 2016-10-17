#ifndef VTKUTILS_GLOBAL_H
#define VTKUTILS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VTKUTILS_LIBRARY)
#  define VTKUTILS_EXPORT Q_DECL_EXPORT
#else
#  define VTKUTILS_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILS_GLOBAL_H
