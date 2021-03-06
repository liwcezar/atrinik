/*************************************************************************
 *           Atrinik, a Multiplayer Online Role Playing Game             *
 *                                                                       *
 *   Copyright (C) 2009-2014 Alex Tokar and Atrinik Development Team     *
 *                                                                       *
 * Fork from Crossfire (Multiplayer game for X-windows).                 *
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
 * Handles code for @ref WAND "wands".
 *
 * @author Alex Tokar
 */

#include <global.h>
#include <object.h>
#include <object_methods.h>

/** @copydoc object_methods_t::ranged_fire_func */
static int ranged_fire_func(object *op, object *shooter, int dir, double *delay)
{
    HARD_ASSERT(op != NULL);
    HARD_ASSERT(shooter != NULL);

    if (op->stats.sp < 0 || op->stats.sp >= NROFREALSPELLS) {
        draw_info_format(COLOR_WHITE, shooter,
                         "The %s is broken.",
                         op->name);
        return OBJECT_METHOD_UNHANDLED;
    }

    if (op->stats.food <= 0) {
        play_sound_player_only(CONTR(shooter),
                               CMD_SOUND_EFFECT,
                               "rod.ogg",
                               0,
                               0,
                               0,
                               0);
        draw_info_format(COLOR_WHITE, shooter,
                         "The %s says poof.",
                         op->name);
        return OBJECT_METHOD_UNHANDLED;
    }

    if (cast_spell(shooter, op, dir, op->stats.sp, 0, CAST_WAND, NULL)) {
        SET_FLAG(op, FLAG_BEEN_APPLIED);
        op->stats.food--;
    }

    if (delay != NULL) {
        *delay = op->last_grace;
    }

    return OBJECT_METHOD_OK;
}

/**
 * Initialize the wand type object methods.
 */
OBJECT_TYPE_INIT_DEFINE(wand)
{
    OBJECT_METHODS(WAND)->apply_func = object_apply_item;
    OBJECT_METHODS(WAND)->ranged_fire_func = ranged_fire_func;
}
