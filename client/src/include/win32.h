/************************************************************************
*            Atrinik, a Multiplayer Online Role Playing Game            *
*                                                                       *
*    Copyright (C) 2009-2011 Alex Tokar and Atrinik Development Team    *
*                                                                       *
* Fork from Daimonin (Massive Multiplayer Online Role Playing Game)     *
* and Crossfire (Multiplayer game for X-windows).                       *
*                                                                       *
* This program is free software; you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 2 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program; if not, write to the Free Software           *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
*                                                                       *
* The author can be reached at admin@atrinik.org                        *
************************************************************************/

#ifndef WIN32_H
#define WIN32_H
#ifdef __WIN_32

#define STRICT

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <windowsx.h>
#include <mmsystem.h>
#include <winsock2.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <direct.h>
#include <shellapi.h>

#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define inline __inline

#ifndef MINGW
#	define strdup _strdup
#	define fileno _fileno
#	define unlink _unlink
#	define lseek _lseek
#	define access _access
#	define strtoull _strtoui64
#	define strtok_r strtok_s
#	define F_OK 6
#	define R_OK 6
#	define W_OK 2
	/* Conversion from 'xxx' to 'yyy', possible loss of data */
#	pragma warning(disable: 4244)
	/* Conversion from 'size_t' to 'int', possible loss of data */
#	pragma warning(disable: 4267)
	/* Initializing float f = 0.05; instead of f = 0.05f; */
#	pragma warning(disable: 4305)
#else
#   define strtok_r(_s, _sep, _lasts) (*(_lasts) = strtok((_s), (_sep)))
#endif

/* Doesn't exist, just a plain int */
#ifndef socklen_t
#	define socklen_t int
#endif

#define mkdir(__a, __b) _mkdir(__a)

#define HAVE_STRICMP
#define HAVE_STRNICMP
#define HAVE_SDL
#define HAVE_SDL_IMAGE
#define HAVE_SDL_TTF
#define HAVE_CURL
#define HAVE_ZLIB
#define HAVE_SDL_MIXER

/* Define to the full name of this package. */
#define PACKAGE_NAME "Atrinik Client"
/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0"

#endif
#endif
