#pragma once
#if defined(__WIN32__) || defined(WIN32)

#include "./win32_conio.hpp"

#else
#include "./unix_conio.hpp"
#endif
