/************************************************************************
*            Atrinik, a Multiplayer Online Role Playing Game            *
*                                                                       *
*    Copyright (C) 2009-2012 Alex Tokar and Atrinik Development Team    *
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
 * Functions related to attributes, weight, experience, which concern
 * only living things. */

#include <global.h>

/** When we carry more than this of our weight_limit, we get encumbered. */
#define ENCUMBRANCE_LIMIT 65.0f

/**
 * Sets Str/Dex/con/Wis/Cha/Int/Pow in stats to value, depending on what
 * attr is (STR to POW).
 * @param stats Item to modify. Must not be NULL.
 * @param attr Attribute to change.
 * @param value New value. */
void set_attr_value(living *stats, int attr, sint8 value)
{
	switch (attr)
	{
		case STR:
			stats->Str = value;
			break;

		case DEX:
			stats->Dex = value;
			break;

		case CON:
			stats->Con = value;
			break;

		case WIS:
			stats->Wis = value;
			break;

		case POW:
			stats->Pow = value;
			break;

		case CHA:
			stats->Cha = value;
			break;

		case INT:
			stats->Int = value;
			break;
	}
}

/**
 * Like set_attr_value(), but instead the value (which can be negative)
 * is added to the specified stat.
 *
 * Checking is performed to make sure old value + new value doesn't overflow
 * the stat integer.
 * @param stats Item to modify. Must not be NULL.
 * @param attr Attribute to change.
 * @param value Delta (can be positive). */
void change_attr_value(living *stats, int attr, sint8 value)
{
	sint16 result;

	if (value == 0)
	{
		return;
	}

	result = get_attr_value(stats, attr) + value;

	/* Prevent possible overflow of the stat. */
	if (result > SINT8_MAX || result < SINT8_MIN)
	{
		return;
	}

	set_attr_value(stats, attr, result);
}

/**
 * Gets the value of a stat.
 * @param stats Item from which to get stat.
 * @param attr Attribute to get.
 * @return Specified attribute, 0 if not found.
 * @see set_attr_value(). */
sint8 get_attr_value(living *stats, int attr)
{
	switch (attr)
	{
		case STR:
			return stats->Str;

		case DEX:
			return stats->Dex;

		case CON:
			return stats->Con;

		case WIS:
			return stats->Wis;

		case CHA:
			return stats->Cha;

		case INT:
			return stats->Int;

		case POW:
			return stats->Pow;
	}

	return 0;
}

/**
 * Ensures that all stats (str/dex/con/wis/cha/int) are within the
 * passed in range of MIN_STAT and MAX_STAT.
 * @param stats Attributes to check. */
void check_stat_bounds(living *stats)
{
	int i, v;

	for (i = 0; i < NUM_STATS; i++)
	{
		v = get_attr_value(stats, i);

		if (v > MAX_STAT)
		{
			set_attr_value(stats, i, MAX_STAT);
		}
		else if (v < MIN_STAT)
		{
			set_attr_value(stats, i, MIN_STAT);
		}
	}
}

/**
 * Permanently alters an object's stats/flags based on another object.
 * @return 1 if we successfully changed a stat, 0 if nothing was changed.
 * @note Flag is set to 1 if we are applying the object, -1 if we are
 * removing the object.
 * @note
 * It is the calling functions responsibility to check to see if the object
 * can be applied or not. */
int change_abil(object *op, object *tmp)
{
	int flag = QUERY_FLAG(tmp, FLAG_APPLIED) ? 1 : -1, i, success = 0;
	object refop;

	/* Remember what object was like before it was changed. Note that
	 * refop is a local copy of op only to be used for detecting changes
	 * found by fix_player. refop is not a real object. */
	memcpy(&refop, op, sizeof(object));

	/* Reset attributes that fix_player doesn't reset since it doesn't search
	 * everything to set */
	if (flag == -1)
	{
		op->path_attuned &= ~tmp->path_attuned, op->path_repelled &= ~tmp->path_repelled, op->path_denied &= ~tmp->path_denied;
	}

	/* call fix_player since op object could have whatever attribute due
	 * to multiple items.  if fix_player always has to be called after
	 * change_ability then might as well call it from here */
	fix_player(op);

	if (tmp->attack[ATNR_CONFUSION])
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "Your hands begin to glow red.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "Your hands stop glowing red.");
		}
	}

	if (QUERY_FLAG(op, FLAG_LIFESAVE) != QUERY_FLAG(&refop, FLAG_LIFESAVE))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel very protected.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You don't feel protected anymore.");
		}
	}

	if (QUERY_FLAG(op, FLAG_REFL_MISSILE) != QUERY_FLAG(&refop, FLAG_REFL_MISSILE))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "A magic force shimmers around you.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "The magic force fades away.");
		}
	}

	if (QUERY_FLAG(op, FLAG_REFL_SPELL) != QUERY_FLAG(&refop, FLAG_REFL_SPELL))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel more safe now, somehow.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "Suddenly you feel less safe, somehow.");
		}
	}

	if (QUERY_FLAG(tmp, FLAG_FLYING))
	{
		if (flag > 0)
		{
			success = 1;

			/* If we're already flying then fly higher */
			if (QUERY_FLAG(op, FLAG_FLYING) == QUERY_FLAG(&refop, FLAG_FLYING))
			{
				draw_info(COLOR_WHITE, op, "You float a little higher in the air.");
			}
			else
			{
				draw_info(COLOR_GRAY, op, "You start to float in the air!");

				SET_MULTI_FLAG(op, FLAG_FLYING);

				if (op->speed > 1)
				{
					op->speed = 1;
				}
			}
		}
		else
		{
			success = 1;

			/* If we're already flying then fly lower */
			if (QUERY_FLAG(op, FLAG_FLYING) == QUERY_FLAG(&refop, FLAG_FLYING))
			{
				draw_info(COLOR_WHITE, op, "You float a little lower in the air.");
			}
			else
			{
				draw_info(COLOR_GRAY, op, "You float down to the ground.");
			}
		}
	}

	/* Becoming UNDEAD... a special treatment for this flag. Only those not
	 * originally undead may change their status */
	if (!QUERY_FLAG(&op->arch->clone, FLAG_UNDEAD))
	{
		if (QUERY_FLAG(op, FLAG_UNDEAD) != QUERY_FLAG(&refop, FLAG_UNDEAD))
		{
			success = 1;

			if (flag > 0)
			{
				FREE_AND_COPY_HASH(op->race, "undead");
				draw_info(COLOR_GRAY, op, "Your lifeforce drains away!");
			}
			else
			{
				FREE_AND_CLEAR_HASH(op->race);

				if (op->arch->clone.race)
				{
					FREE_AND_COPY_HASH(op->race, op->arch->clone.race);
				}

				draw_info(COLOR_WHITE, op, "Your lifeforce returns!");
			}
		}
	}

	if (QUERY_FLAG(op, FLAG_STEALTH) != QUERY_FLAG(&refop, FLAG_STEALTH))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You walk more quietly.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You walk more noisily.");
		}
	}

	if (QUERY_FLAG(op, FLAG_SEE_INVISIBLE) != QUERY_FLAG(&refop, FLAG_SEE_INVISIBLE))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You see invisible things.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "Your vision becomes less clear.");
		}
	}

	if (QUERY_FLAG(op, FLAG_IS_INVISIBLE) != QUERY_FLAG(&refop, FLAG_IS_INVISIBLE))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You become transparent.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You can see yourself.");
		}
	}

	/* Blinded you can tell if more blinded since blinded player has minimal
	 * vision */
	if (QUERY_FLAG(tmp, FLAG_BLIND))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_GRAY, op, "You are blinded.");
			SET_FLAG(op, FLAG_BLIND);

			if (op->type == PLAYER)
			{
				CONTR(op)->update_los = 1;
			}
		}
		else
		{
			draw_info(COLOR_WHITE, op, "Your vision returns.");
			CLEAR_FLAG(op, FLAG_BLIND);

			if (op->type == PLAYER)
			{
				CONTR(op)->update_los = 1;
			}
		}
	}

	if (QUERY_FLAG(op, FLAG_SEE_IN_DARK) != QUERY_FLAG(&refop, FLAG_SEE_IN_DARK))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "Your vision is better in the dark.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You see less well in the dark.");
		}
	}

	if (QUERY_FLAG(op, FLAG_XRAYS) != QUERY_FLAG(&refop, FLAG_XRAYS))
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_GRAY, op, "Everything becomes transparent.");

			if (op->type == PLAYER)
			{
				CONTR(op)->update_los = 1;
			}
		}
		else
		{
			draw_info(COLOR_GRAY, op, "Everything suddenly looks very solid.");

			if (op->type == PLAYER)
			{
				CONTR(op)->update_los = 1;
			}
		}
	}

	if ((tmp->stats.hp || tmp->stats.maxhp) && op->type == PLAYER)
	{
		success = 1;

		if ((flag * tmp->stats.hp) > 0 || (flag * tmp->stats.maxhp) > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel much more healthy!");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You feel much less healthy!");
		}
	}

	if ((tmp->stats.sp || tmp->stats.maxsp) && op->type == PLAYER && tmp->type != SKILL)
	{
		success = 1;

		if ((flag * tmp->stats.sp) > 0 || (flag * tmp->stats.maxsp) > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel one with the powers of magic!");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You suddenly feel very mundane.");
		}
	}

	if (tmp->stats.food && !QUERY_FLAG(tmp, FLAG_IS_USED_UP))
	{
		success = 1;

		if ((tmp->stats.food * flag) > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel your digestion slowing down.");
		}
		else
		{
			draw_info(COLOR_GRAY, op, "You feel your digestion speeding up.");
		}
	}

	/* Messages for changed protections */
	for (i = 0; i < NROFATTACKS; i++)
	{
		if (op->protection[i] != refop.protection[i])
		{
			success = 1;

			if (op->protection[i] > refop.protection[i])
			{
				draw_info_format(COLOR_GREEN, op, "Your protection to %s rises to %d%%.", attack_name[i], op->protection[i]);
			}
			else
			{
				draw_info_format(COLOR_BLUE, op, "Your protection to %s drops to %d%%.", attack_name[i], op->protection[i]);
			}
		}
	}

	if (tmp->type == POISONING)
	{
		success = 1;

		if (flag > 0)
		{
			draw_info(COLOR_WHITE, op, "You feel very sick...");
		}
		else
		{
			draw_info(COLOR_WHITE, op, "You feel much better now.");
		}
	}

	return success;
}

static void living_apply_flags(object *op, object *tmp)
{
	op->path_attuned |= tmp->path_attuned;
	op->path_repelled |= tmp->path_repelled;
	op->path_denied |= tmp->path_denied;

	op->terrain_flag |= tmp->terrain_type;

	if (QUERY_FLAG(tmp, FLAG_LIFESAVE))
	{
		SET_FLAG(op, FLAG_LIFESAVE);
	}

	if (QUERY_FLAG(tmp, FLAG_REFL_SPELL))
	{
		SET_FLAG(op, FLAG_REFL_SPELL);
	}

	if (QUERY_FLAG(tmp, FLAG_REFL_MISSILE))
	{
		SET_FLAG(op, FLAG_REFL_MISSILE);
	}

	if (QUERY_FLAG(tmp, FLAG_STEALTH))
	{
		SET_FLAG(op, FLAG_STEALTH);
	}

	if (QUERY_FLAG(tmp, FLAG_XRAYS))
	{
		SET_FLAG(op, FLAG_XRAYS);
	}

	if (QUERY_FLAG(tmp, FLAG_BLIND))
	{
		SET_FLAG(op, FLAG_BLIND);
	}

	if (QUERY_FLAG(tmp, FLAG_SEE_IN_DARK))
	{
		SET_FLAG(op, FLAG_SEE_IN_DARK);
	}

	if (QUERY_FLAG(tmp, FLAG_CAN_PASS_THRU))
	{
		SET_FLAG(op, FLAG_CAN_PASS_THRU);
	}

	if (QUERY_FLAG(tmp, FLAG_MAKE_ETHEREAL))
	{
		SET_FLAG(op, FLAG_CAN_PASS_THRU);
		SET_FLAG(op, FLAG_IS_ETHEREAL);
	}

	if (QUERY_FLAG(tmp, FLAG_FLYING))
	{
		SET_FLAG(op, FLAG_FLYING);
	}
}

/**
 * Updates all abilities given by applied objects in the inventory
 * of the given object.
 *
 * This functions starts from base values (archetype or player object)
 * and then adjusts them according to what the player/monster has equipped.
 *
 * Note that a player always has stats reset to their initial value.
 * @param op Object to reset.
 * @todo This function is too long, and should be cleaned / split. */
void fix_player(object *op)
{
	int ring_count = 0;
	int old_glow, max_boni_hp = 0, max_boni_sp = 0;
	int i, light;
	object *tmp;
	float max = 9, added_speed = 0, bonus_speed = 0, speed_reduce_from_disease = 1;
	player *pl;

	if (QUERY_FLAG(op, FLAG_NO_FIX_PLAYER))
	{
		return;
	}

	if (QUERY_FLAG(op, FLAG_MONSTER) && op->type != PLAYER)
	{
		fix_monster(op);
		return;
	}

	/* For secure */
	if (op->type != PLAYER)
	{
		return;
	}

	pl = CONTR(op);

	pl->digestion = 3;
	pl->gen_hp = 1;
	pl->gen_sp = 1;
	pl->gen_sp_armour = 0;
	pl->item_power = 0;

	op->stats.wc = op->arch->clone.stats.wc;
	op->stats.ac = op->arch->clone.stats.ac;
	op->stats.dam = op->arch->clone.stats.dam;

	op->stats.maxhp = op->arch->clone.stats.maxhp;
	op->stats.maxsp = op->arch->clone.stats.maxsp;

	op->stats.wc_range = op->arch->clone.stats.wc_range;

	old_glow = op->glow_radius;
	light = op->arch->clone.glow_radius;

	op->speed = op->arch->clone.speed;
	op->weapon_speed = 0;
	op->path_attuned = op->arch->clone.path_attuned;
	op->path_repelled = op->arch->clone.path_repelled;
	op->path_denied = op->arch->clone.path_denied;
	/* Reset terrain moving abilities */
	op->terrain_flag = op->arch->clone.terrain_flag;

	FREE_AND_CLEAR_HASH(op->slaying);

	if (!QUERY_FLAG(&op->arch->clone, FLAG_XRAYS))
	{
		CLEAR_FLAG(op, FLAG_XRAYS);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_CAN_PASS_THRU))
	{
		CLEAR_MULTI_FLAG(op, FLAG_CAN_PASS_THRU);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_IS_ETHEREAL))
	{
		CLEAR_MULTI_FLAG(op, FLAG_IS_ETHEREAL);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_LIFESAVE))
	{
		CLEAR_FLAG(op, FLAG_LIFESAVE);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_STEALTH))
	{
		CLEAR_FLAG(op, FLAG_STEALTH);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_BLIND))
	{
		CLEAR_FLAG(op, FLAG_BLIND);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_FLYING))
	{
		CLEAR_MULTI_FLAG(op, FLAG_FLYING);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_REFL_SPELL))
	{
		CLEAR_FLAG(op, FLAG_REFL_SPELL);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_REFL_MISSILE))
	{
		CLEAR_FLAG(op, FLAG_REFL_MISSILE);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_UNDEAD))
	{
		CLEAR_FLAG(op, FLAG_UNDEAD);
	}

	if (!QUERY_FLAG(&op->arch->clone, FLAG_SEE_IN_DARK))
	{
		CLEAR_FLAG(op, FLAG_SEE_IN_DARK);
	}

	/* Initializing player arrays from the values in player archetype clone:  */
	memset(&pl->equipment, 0, sizeof(pl->equipment));
	memset(&pl->skill_ptr, 0, sizeof(pl->skill_ptr));
	memcpy(&op->protection, &op->arch->clone.protection, sizeof(op->protection));
	memcpy(&op->attack, &op->arch->clone.attack, sizeof(op->attack));

	for (tmp = op->inv; tmp; tmp = tmp->below)
	{
		if (QUERY_FLAG(tmp, FLAG_IS_READY))
		{
			pl->equipment[PLAYER_EQUIP_AMMO] = tmp;
			continue;
		}

		if (tmp->type == QUEST_CONTAINER)
		{
			pl->quest_container = tmp;
			continue;
		}

		if (tmp->type == SCROLL || tmp->type == POTION || tmp->type == CONTAINER || tmp->type == LIGHT_REFILL)
		{
			continue;
		}

		if (tmp->glow_radius > light)
		{
			if (tmp->type != LIGHT_APPLY || QUERY_FLAG(tmp, FLAG_APPLIED))
			{
				light = tmp->glow_radius;
			}
		}

		if (tmp->type == SKILL)
		{
			pl->skill_ptr[tmp->stats.sp] = tmp;
		}

		if (QUERY_FLAG(tmp, FLAG_APPLIED))
		{
			if (!IS_ARMOR(tmp) && !IS_WEAPON(tmp) && !OBJECT_IS_RANGED(tmp) && tmp->type != DISEASE && tmp->type != SYMPTOM && tmp->type != POISONING)
			{
				for (i = 0; i < NROFATTACKS; i++)
				{
					if (tmp->protection[i])
					{
						if (tmp->protection[i] < 0)
						{
							op->protection[i] = MAX(SINT8_MIN, op->protection[i] + tmp->protection[i]);
						}
						else if (tmp->protection[i] > op->protection[i])
						{
							op->protection[i] = MIN(tmp->protection[i], op->protection[i] + tmp->protection[i]);
						}
					}

					if (tmp->attack[i])
					{
						op->attack[i] = MIN(UINT8_MAX, MAX(0, op->attack[i] + tmp->attack[i]));
					}
				}

				if (tmp->stats.dam)
				{
					op->stats.dam += tmp->stats.dam + tmp->magic;
				}
			}

			if (tmp->type == POTION_EFFECT)
			{
				living_apply_flags(op, tmp);
			}
			else if (tmp->type == CLASS || tmp->type == FORCE || tmp->type == POISONING || tmp->type == DISEASE || tmp->type == SYMPTOM)
			{
				if (tmp->type == CLASS)
				{
					pl->class_ob = tmp;
				}

				if (ARMOUR_SPEED(tmp) && (float) ARMOUR_SPEED(tmp) / 10.0f < max)
				{
					max = ARMOUR_SPEED(tmp) / 10.0f;
				}

				if (tmp->type != DISEASE && tmp->type != SYMPTOM && tmp->type != POISONING)
				{
					op->stats.maxhp += tmp->stats.maxhp;
					op->stats.maxsp += tmp->stats.maxsp;
				}

				if (tmp->type == DISEASE || tmp->type == SYMPTOM)
				{
					speed_reduce_from_disease = (float) tmp->last_sp / 100.0f;

					if (speed_reduce_from_disease == 0.0f)
					{
						speed_reduce_from_disease = 1.0f;
					}
				}

				living_apply_flags(op, tmp);
			}
			else if (tmp->type == AMULET)
			{
				pl->equipment[PLAYER_EQUIP_AMULET] = tmp;
			}
			else if (tmp->type == WEAPON || OBJECT_IS_RANGED(tmp))
			{
				if (tmp->type == WEAPON && pl->equipment[PLAYER_EQUIP_HAND_MAIN] && (!pl->equipment[PLAYER_EQUIP_HAND_OFF] || (pl->equipment[PLAYER_EQUIP_HAND_OFF]->type == SHIELD && !OBJECT_IS_RANGED(pl->equipment[PLAYER_EQUIP_HAND_MAIN]))))
				{
					pl->equipment[PLAYER_EQUIP_HAND_OFF] = tmp;
				}
				else if (!pl->equipment[PLAYER_EQUIP_HAND_MAIN] || pl->equipment[PLAYER_EQUIP_HAND_MAIN]->type == WEAPON)
				{
					if (QUERY_FLAG(tmp, FLAG_TWO_HANDED))
					{
						pl->equipment[PLAYER_EQUIP_HAND_OFF] = tmp;
					}
					else if (pl->equipment[PLAYER_EQUIP_HAND_MAIN] && pl->equipment[PLAYER_EQUIP_HAND_MAIN]->type == WEAPON && OBJECT_IS_RANGED(tmp))
					{
						pl->equipment[PLAYER_EQUIP_HAND_OFF] = pl->equipment[PLAYER_EQUIP_HAND_MAIN];
					}
					
					pl->equipment[PLAYER_EQUIP_HAND_MAIN] = tmp;
				}
			}
			else if (tmp->type == GLOVES)
			{
				pl->equipment[PLAYER_EQUIP_GAUNTLETS] = tmp;
			}
			else if (tmp->type == RING && ring_count == 0)
			{
				pl->equipment[PLAYER_EQUIP_RING_RIGHT] = tmp;
				ring_count++;
			}
			else if (tmp->type == HELMET)
			{
				pl->equipment[PLAYER_EQUIP_HELM] = tmp;
			}
			else if (tmp->type == ARMOUR)
			{
				pl->equipment[PLAYER_EQUIP_ARMOUR] = tmp;
			}
			else if (tmp->type == GIRDLE)
			{
				pl->equipment[PLAYER_EQUIP_BELT] = tmp;
			}
			else if (tmp->type == GREAVES)
			{
				pl->equipment[PLAYER_EQUIP_GREAVES] = tmp;
			}
			else if (tmp->type == BOOTS)
			{
				pl->equipment[PLAYER_EQUIP_BOOTS] = tmp;
			}
			else if (tmp->type == CLOAK)
			{
				pl->equipment[PLAYER_EQUIP_CLOAK] = tmp;
			}
			else if (tmp->type == BRACERS)
			{
				pl->equipment[PLAYER_EQUIP_BRACERS] = tmp;
			}
			else if (tmp->type == SHIELD)
			{
				if (!pl->equipment[PLAYER_EQUIP_HAND_OFF] || (pl->equipment[PLAYER_EQUIP_HAND_MAIN] && OBJECT_IS_RANGED(pl->equipment[PLAYER_EQUIP_HAND_MAIN]) && pl->equipment[PLAYER_EQUIP_HAND_MAIN]->type == WEAPON))
				{
					pl->equipment[PLAYER_EQUIP_HAND_OFF] = tmp;
				}
			}
			else if (tmp->type == LIGHT_APPLY)
			{
				pl->equipment[PLAYER_EQUIP_LIGHT] = tmp;
			}
			else if (tmp->type == RING && ring_count == 1)
			{
				pl->equipment[PLAYER_EQUIP_RING_LEFT] = tmp;
				ring_count++;
			}
			else
			{
				logger_print(LOG(BUG), "Unexpected applied object ('%s' in %s).", query_name(tmp, op), op->name);
				CLEAR_FLAG(tmp, FLAG_APPLIED);
			}
		}
	}

	for (i = 0; i < PLAYER_EQUIP_MAX; i++)
	{
		if (!pl->equipment[i])
		{
			continue;
		}

		if (i == PLAYER_EQUIP_HAND_OFF && pl->equipment[PLAYER_EQUIP_HAND_MAIN] && QUERY_FLAG(pl->equipment[PLAYER_EQUIP_HAND_MAIN], FLAG_TWO_HANDED))
		{
			pl->equipment[i] = pl->equipment[PLAYER_EQUIP_HAND_MAIN];
			continue;
		}

		if (i == PLAYER_EQUIP_AMMO)
		{
			continue;
		}

		/* Used for ALL armours except rings and amulets */
		if (IS_ARMOR(pl->equipment[i]) && ARMOUR_SPEED(pl->equipment[i]) && (float) ARMOUR_SPEED(pl->equipment[i]) / 10.0f < max)
		{
			max = ARMOUR_SPEED(pl->equipment[i]) / 10.0f;
		}

		if (pl->equipment[i]->stats.exp && pl->equipment[i]->type != SKILL)
		{
			if (pl->equipment[i]->stats.exp > 0)
			{
				added_speed += (float) pl->equipment[i]->stats.exp / 3.0f;
				bonus_speed += 1.0f + (float) pl->equipment[i]->stats.exp / 3.0f;
			}
			else
			{
				added_speed += (float) pl->equipment[i]->stats.exp;
			}
		}

		if (!IS_WEAPON(pl->equipment[i]) && !OBJECT_IS_RANGED(pl->equipment[i]))
		{
			max_boni_hp += pl->equipment[i]->stats.maxhp;
			max_boni_sp += pl->equipment[i]->stats.maxsp;

			pl->digestion += pl->equipment[i]->stats.food;
			pl->gen_sp += pl->equipment[i]->stats.sp;
			pl->gen_hp += pl->equipment[i]->stats.hp;
			pl->gen_sp_armour += pl->equipment[i]->last_heal;
		}
		
		pl->item_power += pl->equipment[i]->item_power;

		living_apply_flags(op, pl->equipment[i]);
	}

	if (added_speed >= 0)
	{
		op->speed += added_speed / 10.0f;
	}
	else
	{
		op->speed /= 1.0f - added_speed;
	}

	if (op->speed > max)
	{
		op->speed = max;
	}

	/* Calculate real speed */
	op->speed += bonus_speed / 10.0f;

	/* Put a lower limit on speed. Note with this speed, you move once every
	 * 100 ticks or so. This amounts to once every 12 seconds of realtime. */
	op->speed = op->speed * speed_reduce_from_disease;

	/* Don't reduce under this value */
	if (op->speed < 0.01f)
	{
		op->speed = 0.01f;
	}
	else if (!pl->tgm)
	{
		/* Max kg we can carry */
		double f = (PLAYER_WEIGHT_LIMIT(pl)) * ENCUMBRANCE_LIMIT;

		if (((sint32) f) <= op->carrying)
		{
			sint32 weight_limit;

			weight_limit = PLAYER_WEIGHT_LIMIT(pl) * 1000.0;

			if (op->carrying >= weight_limit)
			{
				op->speed = 0.01f;
			}
			else
			{
				/* Total encumbrance weight part */
				f = ((double) weight_limit - f);
				/* Value from 0.0 to 1.0 encumbrance */
				f = ((double) weight_limit - op->carrying) / f;

				if (f < 0.0f)
				{
					f = 0.0f;
				}
				else if (f > 1.0f)
				{
					f = 1.0f;
				}

				op->speed *= f;

				if (op->speed < 0.01f)
				{
					op->speed = 0.01f;
				}
			}
		}
	}

	update_ob_speed(op);

	op->glow_radius = light;

	if (op->map && old_glow != light)
	{
		adjust_light_source(op->map, op->x, op->y, light - old_glow);
	}

	op->stats.maxhp *= op->level + 3;
	op->stats.maxsp *= pl->skill_ptr[SK_WIZARDRY_SPELLS] ? pl->skill_ptr[SK_WIZARDRY_SPELLS]->level : 1 + 3;

	op->stats.maxhp += max_boni_hp;
	op->stats.maxsp += max_boni_sp;

	/* HP/SP adjustments coming from class-defining object. */
	if (CONTR(op)->class_ob)
	{
		if (CONTR(op)->class_ob->stats.hp)
		{
			op->stats.maxhp += ((float) op->stats.maxhp / 100.0f) * (float) CONTR(op)->class_ob->stats.hp;
		}

		if (CONTR(op)->class_ob->stats.sp)
		{
			op->stats.maxsp += ((float) op->stats.maxsp / 100.0f) * (float) CONTR(op)->class_ob->stats.sp;
		}
	}

	if (op->stats.maxhp < 1)
	{
		op->stats.maxhp = 1;
	}

	if (op->stats.maxsp < 1)
	{
		op->stats.maxsp = 1;
	}

	if (op->stats.hp == -1)
	{
		op->stats.hp = op->stats.maxhp;
	}

	if (op->stats.sp == -1)
	{
		op->stats.sp = op->stats.maxsp;
	}

	/* Cap the pools to <= max */
	if (op->stats.hp > op->stats.maxhp)
	{
		op->stats.hp = op->stats.maxhp;
	}

	if (op->stats.sp > op->stats.maxsp)
	{
		op->stats.sp = op->stats.maxsp;
	}

	if (pl->equipment[PLAYER_EQUIP_HAND_MAIN] && pl->equipment[PLAYER_EQUIP_HAND_MAIN]->type == WEAPON && pl->equipment[PLAYER_EQUIP_HAND_MAIN]->item_skill)
	{
		op->weapon_speed = pl->equipment[PLAYER_EQUIP_HAND_MAIN]->last_grace;
		op->stats.wc += SKILL_LEVEL(pl, pl->equipment[PLAYER_EQUIP_HAND_MAIN]->item_skill - 1);
		op->stats.dam = (float) op->stats.dam * LEVEL_DAMAGE(SKILL_LEVEL(pl, pl->equipment[PLAYER_EQUIP_HAND_MAIN]->item_skill - 1));
		op->stats.dam *= (float) (pl->equipment[PLAYER_EQUIP_HAND_MAIN]->item_condition) / 100.0f;
	}
	else
	{
		if (pl->skill_ptr[SK_UNARMED])
		{
			op->weapon_speed = pl->skill_ptr[SK_UNARMED]->last_grace;

			for (i = 0; i < NROFATTACKS; i++)
			{
				if (pl->skill_ptr[SK_UNARMED]->attack[i])
				{
					op->attack[i] = MIN(UINT8_MAX, op->attack[i] + pl->skill_ptr[SK_UNARMED]->attack[i]);
				}
			}
		}

		op->stats.wc += SKILL_LEVEL(pl, SK_UNARMED);
		op->stats.dam = (float) op->stats.dam * LEVEL_DAMAGE(SKILL_LEVEL(pl, SK_UNARMED)) / 2;
	}

	if (op->stats.dam < 0)
	{
		op->stats.dam = 0;
	}

	if (!pl->quest_container)
	{
		object *quest_container = get_archetype(QUEST_CONTAINER_ARCHETYPE);

		logger_print(LOG(BUG), "Player %s had no quest container, fixing.", op->name);
		insert_ob_in_ob(quest_container, op);
		pl->quest_container = quest_container;
	}
}

/**
 * Like fix_player(), but for monsters.
 * @param op The monster. */
void fix_monster(object *op)
{
	object *base, *tmp;
	float tmp_add;

	if (op->head)
	{
		return;
	}

	/* Will insert or/and return base info */
	base = insert_base_info_object(op);

	CLEAR_FLAG(op, FLAG_READY_BOW);

	op->stats.maxhp = (base->stats.maxhp * (op->level + 3) + (op->level / 2) * base->stats.maxhp) / 10;
	op->stats.maxsp = base->stats.maxsp * (op->level + 1);

	if (op->stats.hp == -1)
	{
		op->stats.hp = op->stats.maxhp;
	}

	if (op->stats.sp == -1)
	{
		op->stats.sp = op->stats.maxsp;
	}

	/* Cap the pools to <= max */
	if (op->stats.hp > op->stats.maxhp)
	{
		op->stats.hp = op->stats.maxhp;
	}

	if (op->stats.sp > op->stats.maxsp)
	{
		op->stats.sp = op->stats.maxsp;
	}

	op->stats.ac = base->stats.ac + op->level;
	/* + level / 4 to catch up the equipment improvements of
	 * the players in armour items. */
	op->stats.wc = base->stats.wc + op->level + (op->level / 4);
	op->stats.dam = base->stats.dam;

	if (base->stats.wc_range)
	{
		op->stats.wc_range = base->stats.wc_range;
	}
	/* Default value if not set in arch */
	else
	{
		op->stats.wc_range = 20;
	}

	for (tmp = op->inv; tmp; tmp = tmp->below)
	{
		/* Check for bow and use it! */
		if (tmp->type == BOW)
		{
			if (QUERY_FLAG(op, FLAG_USE_BOW))
			{
				SET_FLAG(tmp, FLAG_APPLIED);
				SET_FLAG(op, FLAG_READY_BOW);
			}
			else
			{
				CLEAR_FLAG(tmp, FLAG_APPLIED);
			}
		}
		else if (QUERY_FLAG(op, FLAG_USE_ARMOUR) && IS_ARMOR(tmp) && check_good_armour(op, tmp))
		{
			SET_FLAG(tmp, FLAG_APPLIED);
		}
		else if (QUERY_FLAG(op, FLAG_USE_WEAPON) && tmp->type == WEAPON && check_good_weapon(op, tmp))
		{
			SET_FLAG(tmp, FLAG_APPLIED);
		}

		if (QUERY_FLAG(tmp, FLAG_APPLIED))
		{
			int i;

			if (tmp->type == WEAPON)
			{
				op->stats.dam += tmp->stats.dam;
				op->stats.wc += tmp->stats.wc;
			}
			else if (IS_ARMOR(tmp))
			{
				for (i = 0; i < NROFATTACKS; i++)
				{
					op->protection[i] = MIN(op->protection[i] + tmp->protection[i], 15);
				}

				op->stats.ac += tmp->stats.ac;
			}
		}
	}

	if ((tmp_add = LEVEL_DAMAGE(op->level / 3) - 0.75f) < 0)
	{
		tmp_add = 0;
	}

	if (op->more && QUERY_FLAG(op, FLAG_FRIENDLY))
	{
		SET_MULTI_FLAG(op, FLAG_FRIENDLY);
	}

	op->stats.dam = (sint16) (((float) op->stats.dam * ((LEVEL_DAMAGE(op->level < 0 ? 0 : op->level) + tmp_add) * (0.925f + 0.05 * (op->level / 10)))) / 10.0f);

	/* Add a special decrease of power for monsters level 1-5 */
	if (op->level <= 5)
	{
		float d = 1.0f - ((0.35f / 5.0f) * (float) (6 - op->level));

		op->stats.dam = (int) ((float) op->stats.dam * d);

		if (op->stats.dam < 1)
		{
			op->stats.dam = 1;
		}

		op->stats.maxhp = (int) ((float) op->stats.maxhp * d);

		if (op->stats.maxhp < 1)
		{
			op->stats.maxhp = 1;
		}

		if (op->stats.hp > op->stats.maxhp)
		{
			op->stats.hp = op->stats.maxhp;
		}
	}

	set_mobile_speed(op, 0);

	if (QUERY_FLAG(op, FLAG_ANIMATE))
	{
		animate_object(op, 0);
	}
}

/**
 * Insert and initialize base info object in object op.
 * @param op Object.
 * @return Pointer to the inserted base info object. */
object *insert_base_info_object(object *op)
{
	object *tmp, *head = op, *env;

	if (op->head)
	{
		head = op->head;
	}

	if (op->type == PLAYER)
	{
		logger_print(LOG(BUG), "Try to inserting base_info in player %s!", query_name(head, NULL));
		return NULL;
	}

	if ((tmp = find_base_info_object(head)))
	{
		return tmp;
	}

	tmp = get_object();
	tmp->arch = op->arch;
	/* Copy without putting it on active list */
	copy_object(head, tmp, 1);
	tmp->type = BASE_INFO;
	tmp->speed_left = tmp->speed;
	/* Ensure this object will not be active in any way */
	tmp->speed = 0.0f;
	tmp->face = base_info_archetype->clone.face;
	SET_FLAG(tmp, FLAG_NO_DROP);
	CLEAR_FLAG(tmp, FLAG_ANIMATE);
	CLEAR_FLAG(tmp, FLAG_FRIENDLY);
	CLEAR_FLAG(tmp, FLAG_MONSTER);
	/* And put it in the mob */
	insert_ob_in_ob(tmp, head);

	env = get_env_recursive(op);

	/* Store position (for returning home after aggro is lost...) */
	if (env->map)
	{
		tmp->x = env->x;
		tmp->y = env->y;
		FREE_AND_ADD_REF_HASH(tmp->slaying, env->map->path);
	}

	return tmp;
}

/**
 * Find base info object in monster.
 * @param op Monster object.
 * @return Pointer to the base info if found, NULL otherwise. */
object *find_base_info_object(object *op)
{
	object *tmp;

	for (tmp = op->inv; tmp; tmp = tmp->below)
	{
		if (tmp->type == BASE_INFO)
		{
			return tmp;
		}
	}

	return NULL;
}

/**
 * Set the movement speed of a monster.
 * 1/5 = mob is slowed (by magic)
 * 2/5 = normal mob speed - moving normal
 * 3/5 = mob is moving fast
 * 4/5 = mov is running/attack speed
 * 5/5 = mob is hasted and moving full speed
 * @param op Monster.
 * @param idx Index. */
void set_mobile_speed(object *op, int idx)
{
	object *base;
	float speed, tmp;

	base = insert_base_info_object(op);

	speed = base->speed_left;

	tmp = op->speed;

	if (idx)
	{
		op->speed = speed * idx;
	}
	/* We will generate the speed by setting of the monster */
	else
	{
		/* If not slowed... */
		if (!QUERY_FLAG(op, FLAG_SLOW_MOVE))
		{
			speed += base->speed_left;
		}

		/* Valid enemy - monster is fighting! */
		if (OBJECT_VALID(op->enemy, op->enemy_count))
		{
			speed += base->speed_left * 2;
		}

		op->speed = speed;
	}

	/* Update speed if needed */
	if ((tmp && !op->speed) || (!tmp && op->speed))
	{
		update_ob_speed(op);
	}
}
