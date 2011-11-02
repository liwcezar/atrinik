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
 * Handles code for @ref HANDLE "handles".
 *
 * @author Alex Tokar */

#include <global.h>

/** @copydoc object_methods::apply_func */
static int apply_func(object *op, object *applier, int aflags)
{
	(void) aflags;

	if (op->speed)
	{
		draw_info_format(COLOR_WHITE, applier, "The %s won't budge.", op->name);
		return OBJECT_METHOD_OK;
	}

	/* Toggle the state. */
	op->value = !op->value;
	SET_ANIMATION(op, ((NUM_ANIMATIONS(op) / NUM_FACINGS(op)) * op->direction) + op->value);
	update_object(op, UP_OBJ_FACE);

	/* Inform the applier. */
	draw_info_format(COLOR_WHITE, applier, "You turn the %s.", op->name);
	play_sound_map(op->map, CMD_SOUND_EFFECT, "pull.ogg", op->x, op->y, 0, 0);

	connection_trigger(op, op->value);

	if (QUERY_FLAG(op, FLAG_STAND_STILL))
	{
		if (op->stats.exp)
		{
			op->speed = 1.0 / op->stats.exp;
		}
		else
		{
			op->speed = 1.0;
		}

		update_ob_speed(op);
		op->speed_left = -1;
	}

	return OBJECT_METHOD_OK;
}

/** @copydoc object_methods::trigger_func */
static int trigger_func(object *op, object *cause, int state)
{
	(void) cause;

	if (op->speed)
	{
		return OBJECT_METHOD_OK;
	}

	op->value = state;
	SET_ANIMATION(op, ((NUM_ANIMATIONS(op) / NUM_FACINGS(op)) * op->direction) + op->value);
	update_object(op, UP_OBJ_FACE);

	return OBJECT_METHOD_OK;
}

/** @copydoc object_methods::process_func */
static void process_func(object *op)
{
	op->value = 0;
	SET_ANIMATION(op, ((NUM_ANIMATIONS(op) / NUM_FACINGS(op)) * op->direction) + op->value);
	update_object(op, UP_OBJ_FACE);

	op->speed = 0;
	update_ob_speed(op);
}

/**
 * Initialize the handle type object methods. */
void object_type_init_handle(void)
{
	object_type_methods[HANDLE].apply_func = apply_func;
	object_type_methods[HANDLE].trigger_func = trigger_func;
	object_type_methods[HANDLE].process_func = process_func;
}
