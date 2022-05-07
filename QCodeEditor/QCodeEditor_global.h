#pragma once
#include <qglobal.h>

#ifndef BUILD_STATIC
# if defined(QCODEEDITOR_LIB)
#  define QCODEEDITOR_EXPORT Q_DECL_EXPORT
# else
#  define QCODEEDITOR_EXPORT Q_DECL_IMPORT
# endif
#else
# define QCODEEDITOR_EXPORT
#endif
