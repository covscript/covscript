#pragma once
#if defined(_WIN32) || defined(WIN32)
/*
 * Dirent interface for Microsoft Visual Studio
 * Version 1.23.1
 *
 * Copyright (C) 2006-2012 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#include <covscript_impl/dirent/dirent.h>

#else

#include <dirent.h>

#endif