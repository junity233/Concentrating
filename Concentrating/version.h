#pragma once

#ifdef _MSC_VER

#define COMPILER "MSVC"

#ifdef _WIN32
#ifdef _WIN64

#define SYSTEM "Win64"

#else

#define SYSTEM "Win32"

#endif

#endif

#endif

#define COMPILE_DATETIME __DATE__ __TIME__