#ifndef QTORMLIB_GLOBAL_H
#define QTORMLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QTORMLIB_LIB
# define QTORMLIB_EXPORT Q_DECL_EXPORT
#else
# define QTORMLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // QTORMLIB_GLOBAL_H
