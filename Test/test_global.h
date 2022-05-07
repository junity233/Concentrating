#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TEST_LIB)
#  define TEST_EXPORT Q_DECL_EXPORT
# else
#  define TEST_EXPORT Q_DECL_IMPORT
# endif
#else
# define TEST_EXPORT
#endif
