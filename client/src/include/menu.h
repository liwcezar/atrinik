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

/**
 * @file
 * Menus header file. */

#ifndef MENU_H
#define MENU_H

/**
 * @defgroup MENU_xxx Menu types
 * The menu types.
 *@{*/
/** No menu. */
#define MENU_NO 1
/** Book GUI. */
#define MENU_BOOK 64
#define MENU_REGION_MAP 256
/*@}*/

/** Maximum quickslots in a single group. */
#define MAX_QUICK_SLOTS 8
/** Maximum quickslot groups. */
#define MAX_QUICKSLOT_GROUPS 4

/** One quickslot. */
typedef struct _quickslot
{
	/** If this is item, what tag ID. */
	int tag;

	spell_entry_struct *spell;
} _quickslot;

extern int quickslot_group;

#endif
