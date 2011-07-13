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
 * Atrinik python plugin. */

#ifdef WIN32
#	include <fcntl.h>
#endif

#include <plugin_python.h>

#include <compile.h>
#include <eval.h>
#ifdef STR
/* STR is redefined in node.h. Since this file doesn't use STR, we remove it */
#undef STR
#endif
#include <node.h>

/** Hooks. */
struct plugin_hooklist *hooks;

/** A generic exception that we use for error messages. */
PyObject *AtrinikError;

/** The context stack. */
static PythonContext *context_stack;
/** Current context. */
PythonContext *current_context;

/**
 * Useful constants */
/* @cparser
 * @page plugin_python_constants Python constants
 * <h2>Python constants</h2>
 * List of the Python plugin constants and their meaning. */
static const Atrinik_Constant constants[] =
{
	{"NORTH", NORTH},
	{"NORTHEAST", NORTHEAST},
	{"EAST", EAST},
	{"SOUTHEAST", SOUTHEAST},
	{"SOUTH", SOUTH},
	{"SOUTHWEST", SOUTHWEST},
	{"WEST", WEST},
	{"NORTHWEST", NORTHWEST},

	{"llevSystem", llevSystem},
	{"llevError", llevError},
	{"llevBug", llevBug},
	{"llevInfo", llevInfo},
	{"llevDebug", llevDebug},
	{"llevChat", llevChat},

	{"PLUGIN_EVENT_NORMAL", PLUGIN_EVENT_NORMAL},
	{"PLUGIN_EVENT_GLOBAL", PLUGIN_EVENT_GLOBAL},
	{"PLUGIN_EVENT_MAP", PLUGIN_EVENT_MAP},

	{"EVENT_APPLY", EVENT_APPLY},
	{"EVENT_ATTACK", EVENT_ATTACK},
	{"EVENT_DEATH", EVENT_DEATH},
	{"EVENT_DROP", EVENT_DROP},
	{"EVENT_PICKUP", EVENT_PICKUP},
	{"EVENT_SAY", EVENT_SAY},
	{"EVENT_STOP", EVENT_STOP},
	{"EVENT_TIME", EVENT_TIME},
	{"EVENT_THROW", EVENT_THROW},
	{"EVENT_TRIGGER", EVENT_TRIGGER},
	{"EVENT_CLOSE", EVENT_CLOSE},
	{"EVENT_TIMER", EVENT_TIMER},
	{"EVENT_ASK_SHOW", EVENT_ASK_SHOW},

	{"MEVENT_ENTER", MEVENT_ENTER},
	{"MEVENT_LEAVE", MEVENT_LEAVE},
	{"MEVENT_RESET", MEVENT_RESET},
	{"MEVENT_SPELL_CAST", MEVENT_SPELL_CAST},
	{"MEVENT_SKILL_USED", MEVENT_SKILL_USED},
	{"MEVENT_DROP", MEVENT_DROP},
	{"MEVENT_PICK", MEVENT_PICK},
	{"MEVENT_PUT", MEVENT_PUT},
	{"MEVENT_APPLY", MEVENT_APPLY},
	{"MEVENT_LOGIN", MEVENT_LOGIN},
	{"MEVENT_CMD_DROP", MEVENT_CMD_DROP},
	{"MEVENT_CMD_TAKE", MEVENT_CMD_TAKE},
	{"MEVENT_EXAMINE", MEVENT_EXAMINE},

	{"GEVENT_BORN", GEVENT_BORN},
	{"GEVENT_LOGIN", GEVENT_LOGIN},
	{"GEVENT_LOGOUT", GEVENT_LOGOUT},
	{"GEVENT_PLAYER_DEATH", GEVENT_PLAYER_DEATH},
	{"GEVENT_CACHE_REMOVED", GEVENT_CACHE_REMOVED},

	{"MAP_INFO_NORMAL", MAP_INFO_NORMAL},
	{"MAP_INFO_ALL", MAP_INFO_ALL},

	{"COST_TRUE", COST_TRUE},
	{"COST_BUY", COST_BUY},
	{"COST_SELL", COST_SELL},

	{"APPLY_TOGGLE", 0},
	{"APPLY_ALWAYS", AP_APPLY},
	{"UNAPPLY_ALWAYS", AP_UNAPPLY},
	{"UNAPPLY_NO_MERGE", AP_NO_MERGE},
	{"UNAPPLY_IGNORE_CURSE", AP_IGNORE_CURSE},
	{"APPLY_NO_EVENT", AP_NO_EVENT},

	{"MAXLEVEL", MAXLEVEL},

	{"CAST_NORMAL", CAST_NORMAL},
	{"CAST_WAND", CAST_WAND},
	{"CAST_ROD", CAST_ROD},
	{"CAST_HORN", CAST_HORN},
	{"CAST_SCROLL", CAST_SCROLL},
	{"CAST_POTION", CAST_POTION},
	{"CAST_NPC", CAST_NPC},

	{"IDENTIFY_NORMAL", IDENTIFY_NORMAL},
	{"IDENTIFY_ALL", IDENTIFY_ALL},
	{"IDENTIFY_MARKED", IDENTIFY_MARKED},

	{"CLONE_WITH_INVENTORY", 0},
	{"CLONE_WITHOUT_INVENTORY", 1},

	{"EXP_AGILITY", EXP_AGILITY},
	{"EXP_MENTAL", EXP_MENTAL},
	{"EXP_MAGICAL", EXP_MAGICAL},
	{"EXP_PERSONAL", EXP_PERSONAL},
	{"EXP_PHYSICAL", EXP_PHYSICAL},
	{"EXP_WISDOM", EXP_WISDOM},

	{"NDI_SAY", NDI_SAY},
	{"NDI_SHOUT", NDI_SHOUT},
	{"NDI_TELL", NDI_TELL},
	{"NDI_PLAYER", NDI_PLAYER},
	{"NDI_ANIM", NDI_ANIM},
	{"NDI_EMOTE", NDI_EMOTE},
	{"NDI_ALL", NDI_ALL},

	{"PLAYER_EQUIP_MAIL", PLAYER_EQUIP_MAIL},
	{"PLAYER_EQUIP_GAUNTLET", PLAYER_EQUIP_GAUNTLET},
	{"PLAYER_EQUIP_BRACER", PLAYER_EQUIP_BRACER},
	{"PLAYER_EQUIP_HELM", PLAYER_EQUIP_HELM},
	{"PLAYER_EQUIP_BOOTS", PLAYER_EQUIP_BOOTS},
	{"PLAYER_EQUIP_CLOAK", PLAYER_EQUIP_CLOAK},
	{"PLAYER_EQUIP_GIRDLE", PLAYER_EQUIP_GIRDLE},
	{"PLAYER_EQUIP_SHIELD", PLAYER_EQUIP_SHIELD},
	{"PLAYER_EQUIP_RRING", PLAYER_EQUIP_RRING},
	{"PLAYER_EQUIP_LRING", PLAYER_EQUIP_LRING},
	{"PLAYER_EQUIP_AMULET", PLAYER_EQUIP_AMULET},
	{"PLAYER_EQUIP_WEAPON", PLAYER_EQUIP_WEAPON},
	{"PLAYER_EQUIP_BOW", PLAYER_EQUIP_BOW},

	{"QUEST_TYPE_SPECIAL", QUEST_TYPE_SPECIAL},
	{"QUEST_TYPE_KILL", QUEST_TYPE_KILL},
	{"QUEST_TYPE_KILL_ITEM", QUEST_TYPE_KILL_ITEM},
	{"QUEST_TYPE_MULTI", QUEST_TYPE_MULTI},
	{"QUEST_STATUS_COMPLETED", QUEST_STATUS_COMPLETED},

	{"PARTY_MESSAGE_STATUS", PARTY_MESSAGE_STATUS},
	{"PARTY_MESSAGE_CHAT", PARTY_MESSAGE_CHAT},

	{"ATNR_IMPACT", ATNR_IMPACT},
	{"ATNR_SLASH", ATNR_SLASH},
	{"ATNR_CLEAVE", ATNR_CLEAVE},
	{"ATNR_PIERCE", ATNR_PIERCE},
	{"ATNR_WEAPON_MAGIC", ATNR_WEAPON_MAGIC},
	{"ATNR_FIRE", ATNR_FIRE},
	{"ATNR_COLD", ATNR_COLD},
	{"ATNR_ELECTRICITY", ATNR_ELECTRICITY},
	{"ATNR_POISON", ATNR_POISON},
	{"ATNR_ACID", ATNR_ACID},
	{"ATNR_MAGIC", ATNR_MAGIC},
	{"ATNR_MIND", ATNR_MIND},
	{"ATNR_BLIND", ATNR_BLIND},
	{"ATNR_PARALYZE", ATNR_PARALYZE},
	{"ATNR_FORCE", ATNR_FORCE},
	{"ATNR_GODPOWER", ATNR_GODPOWER},
	{"ATNR_CHAOS", ATNR_CHAOS},
	{"ATNR_DRAIN", ATNR_DRAIN},
	{"ATNR_SLOW", ATNR_SLOW},
	{"ATNR_CONFUSION", ATNR_CONFUSION},
	{"ATNR_INTERNAL", ATNR_INTERNAL},
	{"NROFATTACKS", NROFATTACKS},

	{"TERRAIN_NOTHING", TERRAIN_NOTHING},
	{"TERRAIN_AIRBREATH", TERRAIN_NOTHING},
	{"TERRAIN_WATERWALK", TERRAIN_WATERWALK},
	{"TERRAIN_WATERBREATH", TERRAIN_WATERBREATH},
	{"TERRAIN_FIREWALK", TERRAIN_FIREWALK},
	{"TERRAIN_FIREBREATH", TERRAIN_FIREBREATH},
	{"TERRAIN_CLOUDWALK", TERRAIN_CLOUDWALK},
	{"TERRAIN_ALL", TERRAIN_ALL},

	{"P_BLOCKSVIEW", P_BLOCKSVIEW},
	{"P_NO_MAGIC", P_NO_MAGIC},
	{"P_NO_PASS", P_NO_PASS},
	{"P_IS_PLAYER", P_IS_PLAYER},
	{"P_IS_ALIVE", P_IS_ALIVE},
	{"P_NO_CLERIC", P_NO_CLERIC},
	{"P_PLAYER_ONLY", P_PLAYER_ONLY},
	{"P_DOOR_CLOSED", P_DOOR_CLOSED},
	{"P_CHECK_INV", P_CHECK_INV},
	{"P_NO_PVP", P_NO_PVP},
	{"P_PASS_THRU", P_PASS_THRU},
	{"P_MAGIC_EAR", P_MAGIC_EAR},
	{"P_WALK_ON", P_WALK_ON},
	{"P_WALK_OFF", P_WALK_OFF},
	{"P_FLY_OFF", P_FLY_OFF},
	{"P_FLY_ON", P_FLY_ON},
	{"P_MAGIC_MIRROR", P_MAGIC_MIRROR},
	{"P_OUTDOOR", P_OUTDOOR},
	{"P_OUT_OF_MAP", P_OUT_OF_MAP},
	{"P_FLAGS_ONLY", P_FLAGS_ONLY},
	{"P_FLAGS_UPDATE", P_FLAGS_UPDATE},
	{"P_NEED_UPDATE", P_NEED_UPDATE},
	{"P_NO_ERROR", P_NO_ERROR},
	{"P_NO_TERRAIN", P_NO_TERRAIN},

	{"CMD_SOUND_EFFECT", CMD_SOUND_EFFECT},
	{"CMD_SOUND_BACKGROUND", CMD_SOUND_BACKGROUND},
	{"CMD_SOUND_ABSOLUTE", CMD_SOUND_ABSOLUTE},

	{"BANK_SYNTAX_ERROR", BANK_SYNTAX_ERROR},
	{"BANK_SUCCESS", BANK_SUCCESS},

	{"BANK_WITHDRAW_HIGH", BANK_WITHDRAW_HIGH},
	{"BANK_WITHDRAW_MISSING", BANK_WITHDRAW_MISSING},
	{"BANK_WITHDRAW_OVERWEIGHT", BANK_WITHDRAW_OVERWEIGHT},

	{"BANK_DEPOSIT_COPPER", BANK_DEPOSIT_COPPER},
	{"BANK_DEPOSIT_SILVER", BANK_DEPOSIT_SILVER},
	{"BANK_DEPOSIT_GOLD", BANK_DEPOSIT_GOLD},
	{"BANK_DEPOSIT_MITHRIL", BANK_DEPOSIT_MITHRIL},

	{"AROUND_ALL", AROUND_ALL},
	{"AROUND_WALL", AROUND_WALL},
	{"AROUND_BLOCKSVIEW", AROUND_BLOCKSVIEW},
	{"AROUND_PLAYER_ONLY", AROUND_PLAYER_ONLY},

	{"LAYER_SYS", LAYER_SYS},
	{"LAYER_FLOOR", LAYER_FLOOR},
	{"LAYER_FMASK", LAYER_FMASK},
	{"LAYER_ITEM", LAYER_ITEM},
	{"LAYER_ITEM2", LAYER_ITEM2},
	{"LAYER_WALL", LAYER_WALL},
	{"LAYER_LIVING", LAYER_LIVING},
	{"LAYER_EFFECT", LAYER_EFFECT},

	{"INVENTORY_ONLY", INVENTORY_ONLY},
	{"INVENTORY_CONTAINERS", INVENTORY_CONTAINERS},
	{"INVENTORY_ALL", INVENTORY_ALL},

	{"GT_ENVIRONMENT", GT_ENVIRONMENT},
	{"GT_INVISIBLE", GT_INVISIBLE},
	{"GT_STARTEQUIP", GT_STARTEQUIP},
	{"GT_APPLY", GT_APPLY},
	{"GT_ONLY_GOOD", GT_ONLY_GOOD},
	{"GT_UPDATE_INV", GT_UPDATE_INV},
	{"GT_NO_VALUE", GT_NO_VALUE},

	{"SIZEOFFREE", SIZEOFFREE},
	{"SIZEOFFREE1", SIZEOFFREE1},
	{"SIZEOFFREE2", SIZEOFFREE2},

	{"NROFREALSPELLS", NROFREALSPELLS},

	{NULL, 0}
};
/* @endcparser */

/** Game object type constants. */
/* @cparser
 * @page plugin_python_constants_types Python game object type constants
 * <h2>Python game object type constants</h2>
 * List of the Python plugin game object type constants and their meaning. */
static const Atrinik_Constant constants_types[] =
{
	{"PLAYER", PLAYER},
	{"BULLET", BULLET},
	{"ROD", ROD},
	{"TREASURE", TREASURE},
	{"POTION", POTION},
	{"FOOD", FOOD},
	{"POISON", POISON},
	{"BOOK", BOOK},
	{"CLOCK", CLOCK},
	{"LIGHTNING", LIGHTNING},
	{"ARROW", ARROW},
	{"BOW", BOW},
	{"WEAPON", WEAPON},
	{"ARMOUR", ARMOUR},
	{"PEDESTAL", PEDESTAL},
	{"ALTAR", ALTAR},
	{"CONFUSION", CONFUSION},
	{"DOOR", DOOR},
	{"KEY", KEY},
	{"MAP", MAP},
	{"MMISSILE", MMISSILE},
	{"TIMED_GATE", TIMED_GATE},
	{"TRIGGER", TRIGGER},
	{"MAGIC_EAR", MAGIC_EAR},
	{"TRIGGER_BUTTON", TRIGGER_BUTTON},
	{"TRIGGER_ALTAR", TRIGGER_ALTAR},
	{"TRIGGER_PEDESTAL", TRIGGER_PEDESTAL},
	{"SHIELD", SHIELD},
	{"HELMET", HELMET},
	{"HORN", HORN},
	{"MONEY", MONEY},
	{"CLASS", CLASS},
	{"GRAVESTONE", GRAVESTONE},
	{"AMULET", AMULET},
	{"PLAYERMOVER", PLAYERMOVER},
	{"TELEPORTER", TELEPORTER},
	{"CREATOR", CREATOR},
	{"SKILL", SKILL},
	{"EXPERIENCE", EXPERIENCE},
	{"BOMB", BOMB},
	{"THROWN_OBJ", THROWN_OBJ},
	{"BLINDNESS", BLINDNESS},
	{"GOD", GOD},
	{"DETECTOR", DETECTOR},
	{"SKILL_ITEM", SKILL_ITEM},
	{"DEAD_OBJECT", DEAD_OBJECT},
	{"DRINK", DRINK},
	{"MARKER", MARKER},
	{"HOLY_ALTAR", HOLY_ALTAR},
	{"PEARL", PEARL},
	{"GEM", GEM},
	{"FIREWALL", FIREWALL},
	{"CHECK_INV", CHECK_INV},
	{"MOOD_FLOOR", MOOD_FLOOR},
	{"EXIT", EXIT},
	{"SHOP_FLOOR", SHOP_FLOOR},
	{"SHOP_MAT", SHOP_MAT},
	{"RING", RING},
	{"FLOOR", FLOOR},
	{"FLESH", FLESH},
	{"INORGANIC", INORGANIC},
	{"LIGHT_APPLY", LIGHT_APPLY},
	{"LIGHTER", LIGHTER},
	{"WALL", WALL},
	{"LIGHT_SOURCE", LIGHT_SOURCE},
	{"MISC_OBJECT", MISC_OBJECT},
	{"MONSTER", MONSTER},
	{"SPAWN_POINT", SPAWN_POINT},
	{"LIGHT_REFILL", LIGHT_REFILL},
	{"SPAWN_POINT_MOB", SPAWN_POINT_MOB},
	{"SPAWN_POINT_INFO", SPAWN_POINT_INFO},
	{"SPELLBOOK", SPELLBOOK},
	{"ORGANIC", ORGANIC},
	{"CLOAK", CLOAK},
	{"CONE", CONE},
	{"SPINNER", SPINNER},
	{"GATE", GATE},
	{"BUTTON", BUTTON},
	{"HANDLE", HANDLE},
	{"PIT", PIT},
	{"TRAPDOOR", TRAPDOOR},
	{"WORD_OF_RECALL", WORD_OF_RECALL},
	{"SIGN", SIGN},
	{"BOOTS", BOOTS},
	{"GLOVES", GLOVES},
	{"BASE_INFO", BASE_INFO},
	{"RANDOM_DROP", RANDOM_DROP},
	{"CONVERTER", CONVERTER},
	{"BRACERS", BRACERS},
	{"POISONING", POISONING},
	{"SAVEBED", SAVEBED},
	{"WAND", WAND},
	{"ABILITY", ABILITY},
	{"SCROLL", SCROLL},
	{"DIRECTOR", DIRECTOR},
	{"GIRDLE", GIRDLE},
	{"FORCE", FORCE},
	{"POTION_EFFECT", POTION_EFFECT},
	{"JEWEL", JEWEL},
	{"NUGGET", NUGGET},
	{"EVENT_OBJECT", EVENT_OBJECT},
	{"WAYPOINT_OBJECT", WAYPOINT_OBJECT},
	{"QUEST_CONTAINER", QUEST_CONTAINER},
	{"CLOSE_CON", CLOSE_CON},
	{"CONTAINER", CONTAINER},
	{"ARMOUR_IMPROVER", ARMOUR_IMPROVER},
	{"WEAPON_IMPROVER", WEAPON_IMPROVER},
	{"WEALTH", WEALTH},
	{"SKILLSCROLL", SKILLSCROLL},
	{"DEEP_SWAMP", DEEP_SWAMP},
	{"IDENTIFY_ALTAR", IDENTIFY_ALTAR},
	{"SWARM_SPELL", SWARM_SPELL},
	{"RUNE", RUNE},
	{"POWER_CRYSTAL", POWER_CRYSTAL},
	{"CORPSE", CORPSE},
	{"DISEASE", DISEASE},
	{"SYMPTOM", SYMPTOM},
	{"MAP_EVENT_OBJ", MAP_EVENT_OBJ},
	{"MAP_INFO", MAP_INFO},
	{"COMPASS", COMPASS},

	{NULL, 0}
};
/* @endcparser */

/** Gender constants. */
/* @cparser
 * @page plugin_python_constants_gender Python gender constants
 * <h2>Python gender constants</h2>
 * List of the Python plugin gender constants and their meaning. */
static const Atrinik_Constant constants_gender[] =
{
	{"NEUTER", GENDER_NEUTER},
	{"MALE", GENDER_MALE},
	{"FEMALE", GENDER_FEMALE},
	{"HERMAPHRODITE", GENDER_HERMAPHRODITE},

	{NULL, 0}
};
/* @endcparser */

/**
 * Color constants */
/* @cparser
 * @page plugin_python_constants_colors Python color constants
 * <h2>Python color constants</h2>
 * List of the Python plugin color constants and their meaning. */
static const char *const constants_colors[][2] =
{
	{"COLOR_WHITE", COLOR_WHITE},
	{"COLOR_ORANGE", COLOR_ORANGE},
	{"COLOR_NAVY", COLOR_NAVY},
	{"COLOR_RED", COLOR_RED},
	{"COLOR_GREEN", COLOR_GREEN},
	{"COLOR_BLUE", COLOR_BLUE},
	{"COLOR_GRAY", COLOR_GRAY},
	{"COLOR_BROWN", COLOR_BROWN},
	{"COLOR_PURPLE", COLOR_PURPLE},
	{"COLOR_PINK", COLOR_PINK},
	{"COLOR_YELLOW", COLOR_YELLOW},
	{"COLOR_DK_NAVY", COLOR_DK_NAVY},
	{"COLOR_DK_GREEN", COLOR_DK_GREEN},
	{"COLOR_DK_ORANGE", COLOR_DK_ORANGE},
	{"COLOR_HGOLD", COLOR_HGOLD},
	{"COLOR_DGOLD", COLOR_DGOLD},
	{NULL, NULL}
};
/* @endcparser */

/** All the custom commands. */
static PythonCmd CustomCommand[NR_CUSTOM_CMD];
/** Contains the index of the next command that needs to be run. */
static int NextCustomCommand;

/** Maximum number of cached scripts. */
#define PYTHON_CACHE_SIZE 256

/** One cache entry. */
typedef struct
{
	/** The script file. */
	const char *file;

	/** The cached code. */
	PyCodeObject *code;

	/** Last cached time. */
	time_t cached_time;

	/** Last used time. */
	time_t used_time;
} cacheentry;

/** The Python cache. */
static cacheentry python_cache[PYTHON_CACHE_SIZE];

static int cmd_customPython(object *op, char *params);

/**
 * Initialize the context stack. */
static void initContextStack()
{
	current_context = NULL;
	context_stack = NULL;
}

/**
 * Push context to the context stack and to current context.
 * @param context The context to push. */
static void pushContext(PythonContext *context)
{
	if (current_context == NULL)
	{
		context_stack = context;
		context->down = NULL;
	}
	else
	{
		context->down = current_context;
	}

	current_context = context;
}

/**
 * Pop the first context from the current context, replacing it by the
 * next one in the list.
 * @return NULL if there is no current context, the previous current
 * context otherwise. */
static PythonContext *popContext()
{
	PythonContext *oldcontext;

	if (current_context)
	{
		oldcontext = current_context;
		current_context = current_context->down;
		return oldcontext;
	}

	return NULL;
}

/**
 * Free a context.
 * @param context Context to free. */
static void freeContext(PythonContext *context)
{
	free(context);
}

/**
 * @defgroup plugin_python_functions Python functions
 *@{*/

/**
 * <h1>LoadObject(string dump)</h1>
 * Load an object from a string dump, for example, one stored using
 * @ref Atrinik_Object_Save "Save()".
 * @param dump The string dump from which to load the actual object. */
static PyObject *Atrinik_LoadObject(PyObject *self, PyObject *args)
{
	char *dumpob;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &dumpob))
	{
		return NULL;
	}

	return wrap_object(hooks->load_object_str(dumpob));
}

/**
 * <h1>ReadyMap(string path, int [unique = False])</h1>
 * Make sure the named map is loaded into memory, loading it if necessary.
 * @param path Path to the map.
 * @param unique Whether the destination should be loaded as unique map,
 * for example, apartments.
 * @return The loaded map. */
static PyObject *Atrinik_ReadyMap(PyObject *self, PyObject *args)
{
	const char *path;
	int flags = 0, unique = 0;

	(void) self;

	if (!PyArg_ParseTuple(args, "s|i", &path, &unique))
	{
		return NULL;
	}

	if (unique)
	{
		flags |= MAP_PLAYER_UNIQUE;
	}

	return wrap_map(hooks->ready_map_name(path, flags));
}

/**
 * <h1>FindPlayer(string name)</h1>
 * Find a player by name.
 * @param name The player name to find.
 * @return The player's object if found, None otherwise. */
static PyObject *Atrinik_FindPlayer(PyObject *self, PyObject *args)
{
	const char *name;
	player *pl;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}

	pl = hooks->find_player(name);

	if (pl)
	{
		return wrap_object(pl->ob);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * <h1>PlayerExists(string name)</h1>
 * Check if player exists.
 * @param name The player name to check.
 * @return True if the player exists, False otherwise */
static PyObject *Atrinik_PlayerExists(PyObject *self, PyObject *args)
{
	const char *name;
	char *cp;
	int ret;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}

	cp = hooks->strdup_local(name);
	hooks->adjust_player_name(cp);
	ret = hooks->player_exists(cp);
	free(cp);

	Py_ReturnBoolean(ret);
}

/**
 * <h1>WhoAmI()</h1>
 * Get the owner of the active script (the object that has the event
 * handler).
 * @return The script owner. */
static PyObject *Atrinik_WhoAmI(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return wrap_object(current_context->who);
}

/**
 * <h1>WhoIsActivator()</h1>
 * Get the object that activated the current event.
 * @return The script activator. */
static PyObject *Atrinik_WhoIsActivator(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return wrap_object(current_context->activator);
}

/**
 * <h1>WhoIsOther()</h1>
 * Get another object related to the event. What this object is depends
 * on the event.
 * @return The other object. */
static PyObject *Atrinik_WhoIsOther(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return wrap_object(current_context->other);
}

/**
 * <h1>WhatIsEvent()</h1>
 * Get the event object that caused this event to trigger.
 * @return The event object. */
static PyObject *Atrinik_WhatIsEvent(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return wrap_object(current_context->event);
}

/**
 * <h1>GetEventNumber()</h1>
 * Get the ID of the event that is being triggered.
 * @return Event ID. */
static PyObject *Atrinik_GetEventNumber(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return Py_BuildValue("i", current_context->event->sub_type);
}

/**
 * <h1>WhatIsMessage()</h1>
 * Gets the actual message in SAY events.
 * @return The message. */
static PyObject *Atrinik_WhatIsMessage(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return Py_BuildValue("s", current_context->text);
}

/**
 * <h1>GetOptions()</h1>
 * Gets the script options (as passed in the event's slaying field).
 * @return The script options. */
static PyObject *Atrinik_GetOptions(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return Py_BuildValue("s", current_context->options);
}

/**
 * <h1>GetReturnValue()</h1>
 * Gets the script's return value.
 * @return The return value. */
static PyObject *Atrinik_GetReturnValue(PyObject *self, PyObject *args)
{
	(void) self;
	(void) args;
	return Py_BuildValue("i", current_context->returnvalue);
}

/**
 * <h1>SetReturnValue(int value)</h1>
 * Sets the script's return value.
 * @param value The new return value. */
static PyObject *Atrinik_SetReturnValue(PyObject *self, PyObject *args)
{
	int value;

	(void) self;

	if (!PyArg_ParseTuple(args, "i", &value))
	{
		return NULL;
	}

	current_context->returnvalue = value;

	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * <h1>GetEventParameters()</h1>
 * Get the parameters of an event. This varies from event to event, and
 * some events pass all parameters as 0. EVENT_ATTACK usually passes damage
 * done and the WC of the hit as second and third parameter, respectively.
 * @return A list of the event parameters. The last entry is the event flags,
 * used to determine whom to call fix_player() on after executing the script. */
static PyObject *Atrinik_GetEventParameters(PyObject *self, PyObject *args)
{
	size_t i;
	PyObject *list = PyList_New(0);

	(void) self;
	(void) args;

	for (i = 0; i < sizeof(current_context->parms) / sizeof(current_context->parms[0]); i++)
	{
		PyList_Append(list, Py_BuildValue("i", current_context->parms[i]));
	}

	return list;
}

/**
 * <h1>GetSpellNr(string name)</h1>
 * Get the ID of the passed spell name.
 * @param name The spell name.
 * @return ID of the spell, -1 if no such spell exists. */
static PyObject *Atrinik_GetSpellNr(PyObject *self, PyObject *args)
{
	const char *spell;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &spell))
	{
		return NULL;
	}

	return Py_BuildValue("i", hooks->look_up_spell_name(spell));
}

/**
 * <h1>GetSpell(int spell)</h1>
 * Get various information about a spell, including things like its
 * level, type, etc.
 * @param spell ID of the spell, can be acquired using @ref Atrinik_GetSpellNr "GetSpellNr()".
 * @throws ValueError if the spell ID being looked up is invalid.
 * @return Dictionary containing information about the spell, with the
 * following entries:
 * - <b>name</b>: Name of the spell.
 * - <b>level</b>: Level required to cast the spell.
 * - <b>type</b>: 'wizard' if the spell is a wizard spell, 'priest' otherwise.
 * - <b>sp</b>: Base mana/grace required to cast the spell; modified by various factors.
 * - <b>time</b>: Delay in ticks needed to cast another spell. */
static PyObject *Atrinik_GetSpell(PyObject *self, PyObject *args)
{
	int spell;
	PyObject *dict;

	(void) self;

	if (!PyArg_ParseTuple(args, "i", &spell))
	{
		return NULL;
	}

	if (spell < 0 || spell >= NROFREALSPELLS)
	{
		PyErr_SetString(PyExc_ValueError, "Invalid ID of a spell.");
		return NULL;
	}

	dict = PyDict_New();

	PyDict_SetItemString(dict, "name", Py_BuildValue("s", hooks->spells[spell].name));
	PyDict_SetItemString(dict, "level", Py_BuildValue("i", hooks->spells[spell].level));
	PyDict_SetItemString(dict, "type", Py_BuildValue("s", hooks->spells[spell].type == SPELL_TYPE_WIZARD ? "wizard" : "priest"));
	PyDict_SetItemString(dict, "sp", Py_BuildValue("i", hooks->spells[spell].sp));
	PyDict_SetItemString(dict, "time", Py_BuildValue("i", hooks->spells[spell].time));

	return dict;
}

/**
 * <h1>GetSkillNr(string name)</h1>
 * Get the ID of the skill.
 * @param name The skill name.
 * @return ID of the skill, -1 if no such skill exists. */
static PyObject *Atrinik_GetSkillNr(PyObject *self, PyObject *args)
{
	const char *skill;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &skill))
	{
		return NULL;
	}

	return Py_BuildValue("i", hooks->lookup_skill_by_name(skill));
}

/**
 * <h1>RegisterCommand(string name, string path, float speed)</h1>
 * Register a custom command ran using Python script.
 * @param name Name of the command. For example, "roll" in order to create /roll
 * command. Note the lack forward slash in the name.
 * @param path Path to the Python script to be executed when the command is used.
 * @param speed How long it takes to execute the command; 1.0 is usually fine.
 * @throws ValueError if the command is already registered. */
static PyObject *Atrinik_RegisterCommand(PyObject *self, PyObject *args)
{
	const char *name, *path;
	double speed;
	size_t i;

	(void) self;

	if (!PyArg_ParseTuple(args, "ssd", &name, &path, &speed))
	{
		return NULL;
	}

	for (i = 0; i < NR_CUSTOM_CMD; i++)
	{
		if (CustomCommand[i].name)
		{
			if (!strcmp(CustomCommand[i].name, name))
			{
				PyErr_SetString(PyExc_ValueError, "RegisterCommand(): Command is already registered.");
				return NULL;
			}
		}
	}

	for (i = 0; i < NR_CUSTOM_CMD; i++)
	{
		if (!CustomCommand[i].name)
		{
			CustomCommand[i].name = hooks->strdup_local(name);
			CustomCommand[i].script = hooks->strdup_local(path);
			CustomCommand[i].speed = speed;
			break;
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * <h1>CreatePathname(string path)</h1>
 * Creates path to file in the maps directory using the create_pathname()
 * function.
 * @param path Path to file to create.
 * @return The path to file in the maps directory. */
static PyObject *Atrinik_CreatePathname(PyObject *self, PyObject *args)
{
	const char *path;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &path))
	{
		return NULL;
	}

	return Py_BuildValue("s", hooks->create_pathname(path));
}

/**
 * <h1>GetTime()</h1>
 * Get the game time.
 * @return A dictionary containing all the information about the in-game
 * time:
 * - <b>year</b>: Current year.
 * - <b>month</b>: Current month.
 * - <b>month_name</b>: Name of the current month.
 * - <b>day</b>: Day.
 * - <b>hour</b>: Hour.
 * - <b>minute</b>: Minute.
 * - <b>dayofweek</b>: Day of the week.
 * - <b>dayofweek_name</b>: Name of the week day.
 * - <b>season</b>: Season.
 * - <b>season_name</b>: Name of the season.
 * - <b>periodofday</b>: Period of the day.
 * - <b>periodofday_name</b>: Name of the period of the day. */
static PyObject *Atrinik_GetTime(PyObject *self, PyObject *args)
{
	PyObject *dict = PyDict_New();
	timeofday_t tod;

	(void) self;
	(void) args;

	hooks->get_tod(&tod);

	PyDict_SetItemString(dict, "year", Py_BuildValue("i", tod.year + 1));
	PyDict_SetItemString(dict, "month", Py_BuildValue("i", tod.month + 1));
	PyDict_SetItemString(dict, "month_name", Py_BuildValue("s", hooks->month_name[tod.month]));
	PyDict_SetItemString(dict, "day", Py_BuildValue("i", tod.day + 1));
	PyDict_SetItemString(dict, "hour", Py_BuildValue("i", tod.hour));
	PyDict_SetItemString(dict, "minute", Py_BuildValue("i", tod.minute));
	PyDict_SetItemString(dict, "dayofweek", Py_BuildValue("i", tod.dayofweek + 1));
	PyDict_SetItemString(dict, "dayofweek_name", Py_BuildValue("s", hooks->weekdays[tod.dayofweek]));
	PyDict_SetItemString(dict, "season", Py_BuildValue("i", tod.season + 1));
	PyDict_SetItemString(dict, "season_name", Py_BuildValue("s", hooks->season_name[tod.season]));
	PyDict_SetItemString(dict, "periodofday", Py_BuildValue("i", tod.periodofday + 1));
	PyDict_SetItemString(dict, "periodofday_name", Py_BuildValue("s", hooks->periodsofday[tod.periodofday]));

	return dict;
}

/**
 * <h1>LocateBeacon(string name)</h1>
 * Locate a beacon.
 * @param name The beacon name to find.
 * @return The beacon if found, None otherwise. */
static PyObject *Atrinik_LocateBeacon(PyObject *self, PyObject *args)
{
	const char *name;
	shstr *beacon_name = NULL;
	object *myob;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}

	FREE_AND_COPY_HASH(beacon_name, name);
	myob = hooks->beacon_locate(beacon_name);
	FREE_AND_CLEAR_HASH(beacon_name);

	return wrap_object(myob);
}

/**
 * <h1>FindParty(string name)</h1>
 * Find a party by name.
 * @param name The party name to find.
 * @return The party if found, None otherwise. */
static PyObject *Atrinik_FindParty(PyObject *self, PyObject *args)
{
	const char *name;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}

	return wrap_party(hooks->find_party(name));
}

/**
 * <h1>CleanupChatString(string text)</h1>
 * Cleans up a chat text removing special characters and extra whitespace.
 * @param text The text to cleanup.
 * @return Cleaned up text; can be None. */
static PyObject *Atrinik_CleanupChatString(PyObject *self, PyObject *args)
{
	const char *text;
	char *cp;
	PyObject *ret;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &text))
	{
		return NULL;
	}

	cp = hooks->strdup_local(text);
	ret = Py_BuildValue("s", hooks->cleanup_chat_string(cp));
	free(cp);

	return ret;
}

/**
 * <h1>LOG(int mode, string message)</h1>
 * Logs a message.
 * @param mode Logging mode to use, one of:
 * - llevError: An irrecoverable error. Will shut down the server.
 * - llevBug: A bug; too many in the same tick will shut down the server.
 * - llevInfo: Info.
 * - llevDebug: Debug information.
 * @param message The message to log. */
static PyObject *Atrinik_LOG(PyObject *self, PyObject *args)
{
	const char *string;
	uint8 mode;

	(void) self;

	if (!PyArg_ParseTuple(args, "Bs", &mode, &string))
	{
		return NULL;
	}

	LOG(mode, string);

	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * <h1>DestroyTimer(int timer)</h1>
 * Destroy an existing timer.
 * @param timer ID of the timer.
 * @return True on success, False on failure. */
static PyObject *Atrinik_DestroyTimer(PyObject *self, PyObject *args)
{
	int id;

	(void) self;

	if (!PyArg_ParseTuple(args, "i", &id))
	{
		return NULL;
	}

	Py_ReturnBoolean(hooks->cftimer_destroy(id) == TIMER_ERR_NONE);
}

/**
 * <h1>GetRangeVectorFromMapCoords(map map, int x, int y, map map2, int x2, int y2, int [flags = 0])</h1>
 * Get the distance and direction from one map coordinate to another.
 * @param map From which map to get distance from.
 * @param x X on 'map'.
 * @param y Y on 'map'.
 * @param map2 Which map to get distance to.
 * @param x2 X on 'map2'.
 * @param y2 Y on 'map2'.
 * @param flags One or a combination of @ref range_vector_flags.
 * @return None if the distance couldn't be calculated, otherwise a tuple
 * containing:
 *  - Direction from the first coordinate to the second, one of @ref direction_constants.
 *  - Distance between the two coordinates.
 *  - X distance.
 *  - Y distance. */
static PyObject *Atrinik_GetRangeVectorFromMapCoords(PyObject *self, PyObject *args)
{
	Atrinik_Map *map, *map2;
	int x, y, x2, y2, flags = 0;
	rv_vector rv;
	PyObject *tuple;

	(void) self;

	if (!PyArg_ParseTuple(args, "O!iiO!ii|i", &Atrinik_MapType, &map, &x, &y, &Atrinik_MapType, &map2, &x2, &y2, &flags))
	{
		return NULL;
	}

	if (!hooks->get_rangevector_from_mapcoords(map->map, x, y, map2->map, x2, y2, &rv, flags))
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	tuple = PyTuple_New(4);
	PyTuple_SET_ITEM(tuple, 0, Py_BuildValue("i", rv.direction));
	PyTuple_SET_ITEM(tuple, 1, Py_BuildValue("i", rv.distance));
	PyTuple_SET_ITEM(tuple, 2, Py_BuildValue("i", rv.distance_x));
	PyTuple_SET_ITEM(tuple, 3, Py_BuildValue("i", rv.distance_y));

	return tuple;
}

/**
 * <h1>CostString(int value)</h1>
 * Build a string representation of the value in the game's money syntax, for
 * example, a value of 134 would become "1 silver coin and 34 copper coins".
 * @param value Value to build the string from.
 * @return The built string. */
static PyObject *Atrinik_CostString(PyObject *self, PyObject *args)
{
	sint64 value;

	(void) self;

	if (!PyArg_ParseTuple(args, "L", &value))
	{
		return NULL;
	}

	return Py_BuildValue("s", hooks->cost_string_from_value(value));
}

/**
 * <h1>CacheAdd(string key, object what)</h1>
 * Store 'what' in memory identified by unique identifier 'key'.
 *
 * The object will be stored forever in memory, until it's either removed by
 * @ref Atrinik_CacheRemove "CacheRemove()" or the server is shut down; in both
 * cases, the object will be closed, if applicable (databases, file objects, etc).
 *
 * A stored object can be retrieved at any time using @ref Atrinik_CacheGet "CacheGet()".
 * @param key The unique identifier for the cache entry.
 * @param what Any Python object (string, integer, database, etc) to store in
 * memory.
 * @return True if the object was cached successfully, False otherwise (cache
 * entry with same key name already exists). */
static PyObject *Atrinik_CacheAdd(PyObject *self, PyObject *args)
{
	const char *key;
	PyObject *what;
	int ret;

	(void) self;

	if (!PyArg_ParseTuple(args, "sO", &key, &what))
	{
		return NULL;
	}

	/* Add it to the cache. */
	ret = hooks->cache_add(key, what, CACHE_FLAG_PYOBJ | CACHE_FLAG_GEVENT);

	if (ret)
	{
		Py_INCREF(what);
	}

	Py_ReturnBoolean(ret);
}

/**
 * <h1>CacheGet(string key)</h1>
 * Attempt to find a cache entry identified by 'key' that was previously
 * added using @ref Atrinik_CacheAdd "CacheAdd()".
 * @param key Unique identifier of the cache entry to find.
 * @throws ValueError if the cache entry could not be found.
 * @return The cache entry. An exception is raised if the cache entry was
 * not found. */
static PyObject *Atrinik_CacheGet(PyObject *self, PyObject *args)
{
	const char *key;
	shstr *sh_key;
	cache_struct *result;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &key))
	{
		return NULL;
	}

	sh_key = hooks->find_string(key);

	/* Even if the cache entry was found, pretend it doesn't exist if
	 * CACHE_FLAG_PYOBJ is not set. */
	if (!sh_key || !(result = hooks->cache_find(sh_key)) || !(result->flags & CACHE_FLAG_PYOBJ))
	{
		PyErr_SetString(PyExc_ValueError, "No such cache entry.");
		return NULL;
	}
	else
	{
		Py_INCREF((PyObject *) result->ptr);
		return (PyObject *) result->ptr;
	}
}

/**
 * <h1>CacheRemove(string key)</h1>
 * Remove a cache entry that was added with a previous call to
 * @ref Atrinik_CacheAdd "CacheAdd()".
 * @param key Unique identifier of the cache entry to remove.
 * @throws ValueError if the cache entry could not be removed (it didn't
 * exist).
 * @return True if the cache entry was removed. An exception is raised if
 * the cache entry was not found. */
static PyObject *Atrinik_CacheRemove(PyObject *self, PyObject *args)
{
	const char *key;
	shstr *sh_key;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &key))
	{
		return NULL;
	}

	sh_key = hooks->find_string(key);

	if (!sh_key || !hooks->cache_remove(sh_key))
	{
		PyErr_SetString(PyExc_ValueError, "No such cache entry.");
		return NULL;
	}
	else
	{
		Py_INCREF(Py_True);
		return Py_True;
	}
}

/**
 * <h1>GetFirst(string what)</h1>
 * Get first member of various linked lists.
 * @param what What list to get first member of. Available list names:
 * - player: First player.
 * - map: First map.
 * - archetype: First archetype.
 * - party: First party.
 * - region: First region.
 * @return First member of the specified linked list. */
static PyObject *Atrinik_GetFirst(PyObject *self, PyObject *args)
{
	const char *what;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &what))
	{
		return NULL;
	}

	if (!strcmp(what, "player"))
	{
		return wrap_player(*hooks->first_player);
	}
	else if (!strcmp(what, "map"))
	{
		return wrap_map(*hooks->first_map);
	}
	else if (!strcmp(what, "archetype"))
	{
		return wrap_archetype(*hooks->first_archetype);
	}
	else if (!strcmp(what, "party"))
	{
		return wrap_party(*hooks->first_party);
	}
	else if (!strcmp(what, "region"))
	{
		return wrap_region(*hooks->first_region);
	}

	PyErr_Format(PyExc_ValueError, "GetFirst(): '%s' is not a valid linked list.", what);
	return NULL;
}

/**
 * <h1>CreateMap(int width, int height, string path)</h1>
 * Creates an empty map.
 * @param width The new map's width.
 * @param height The new map's height.
 * @param path Path to the new map. This should be a unique path to avoid
 * collisions. "/python-maps/" is prepended to this to ensure no collision
 * with regular maps.
 * @return The new empty map. */
static PyObject *Atrinik_CreateMap(PyObject *self, PyObject *args)
{
	int width, height;
	const char *path;
	mapstruct *m;
	char buf[HUGE_BUF];

	(void) self;

	if (!PyArg_ParseTuple(args, "iis", &width, &height, &path))
	{
		return NULL;
	}

	m = hooks->get_empty_map(width, height);
	snprintf(buf, sizeof(buf), "/python-maps/%s", path);
	m->path = hooks->add_string(buf);

	return wrap_map(m);
}

/**
 * <h1>CreateObject(string archname)</h1>
 * Creates a new object. If the created object is not put on map or
 * inside an inventory of another object, it will be removed by the
 * garbage collector.
 * @param archname Name of the arch to create.
 * @throws AtrinikError if 'archname' is not a valid archetype.
 * @return The newly created object. * */
static PyObject *Atrinik_CreateObject(PyObject *self, PyObject *args)
{
	const char *archname;
	archetype *at;

	(void) self;

	if (!PyArg_ParseTuple(args, "s", &archname))
	{
		return NULL;
	}

	at = hooks->find_archetype(archname);

	if (!at)
	{
		PyErr_Format(AtrinikError, "CreateObject(): The archetype '%s' doesn't exist.", archname);
		return NULL;
	}

	return wrap_object(hooks->arch_to_object(at));
}

/*@}*/

/**
 * Here is the Python Declaration Table, used by the interpreter to make
 * an interface with the C code. */
static PyMethodDef AtrinikMethods[] =
{
	{"LoadObject", Atrinik_LoadObject, METH_VARARGS, 0},
	{"ReadyMap", Atrinik_ReadyMap, METH_VARARGS, 0},
	{"FindPlayer", Atrinik_FindPlayer, METH_VARARGS, 0},
	{"PlayerExists", Atrinik_PlayerExists, METH_VARARGS, 0},
	{"WhoAmI", Atrinik_WhoAmI, METH_NOARGS, 0},
	{"WhoIsActivator", Atrinik_WhoIsActivator, METH_NOARGS, 0},
	{"WhoIsOther", Atrinik_WhoIsOther, METH_NOARGS, 0},
	{"WhatIsEvent", Atrinik_WhatIsEvent, METH_NOARGS, 0},
	{"GetEventNumber", Atrinik_GetEventNumber, METH_NOARGS, 0},
	{"WhatIsMessage", Atrinik_WhatIsMessage, METH_NOARGS, 0},
	{"GetOptions", Atrinik_GetOptions, METH_NOARGS, 0},
	{"GetReturnValue", Atrinik_GetReturnValue, METH_NOARGS, 0},
	{"SetReturnValue", Atrinik_SetReturnValue, METH_VARARGS, 0},
	{"GetEventParameters", Atrinik_GetEventParameters, METH_NOARGS, 0},
	{"GetSpellNr", Atrinik_GetSpellNr, METH_VARARGS, 0},
	{"GetSpell", Atrinik_GetSpell, METH_VARARGS, 0},
	{"GetSkillNr", Atrinik_GetSkillNr, METH_VARARGS, 0},
	{"RegisterCommand", Atrinik_RegisterCommand, METH_VARARGS, 0},
	{"CreatePathname", Atrinik_CreatePathname, METH_VARARGS, 0},
	{"GetTime", Atrinik_GetTime, METH_NOARGS, 0},
	{"LocateBeacon", Atrinik_LocateBeacon, METH_VARARGS, 0},
	{"FindParty", Atrinik_FindParty, METH_VARARGS, 0},
	{"CleanupChatString", Atrinik_CleanupChatString, METH_VARARGS, 0},
	{"LOG", Atrinik_LOG, METH_VARARGS, 0},
	{"DestroyTimer", Atrinik_DestroyTimer, METH_VARARGS, 0},
	{"GetRangeVectorFromMapCoords", Atrinik_GetRangeVectorFromMapCoords, METH_VARARGS, 0},
	{"CostString", Atrinik_CostString, METH_VARARGS, 0},
	{"CacheAdd", Atrinik_CacheAdd, METH_VARARGS, 0},
	{"CacheGet", Atrinik_CacheGet, METH_VARARGS, 0},
	{"CacheRemove", Atrinik_CacheRemove, METH_VARARGS, 0},
	{"GetFirst", Atrinik_GetFirst, METH_VARARGS, 0},
	{"CreateMap", Atrinik_CreateMap, METH_VARARGS, 0},
	{"CreateObject", Atrinik_CreateObject, METH_VARARGS, 0},
	{NULL, NULL, 0, 0}
};

/**
 * Log a Python exception. Will also send the exception to any online
 * DMs or those with /resetmap command permission. */
static void PyErr_LOG()
{
	PyObject *globals, *locals, *ret;
	PyObject *ptype, *pvalue, *ptraceback;
	const char *err_handle =
"from Atrinik import *\n"
"import traceback\n"
"exception = \"\".join(traceback.format_exception(exc_type, exc_value, exc_traceback))\n"
"LOG(llevDebug, exception)\n"
"player = GetFirst(\"player\")\n"
"while player:\n"
"	if player.ob.f_wiz or \"resetmap\" in player.cmd_permissions:\n"
"		player.ob.Write(exception, COLOR_RED)\n"
"	player = player.next\n";

	/* Fetch the exception data. */
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);
	PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

	/* Construct globals dictionary. */
	globals = PyDict_New();
	PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());

	/* Construct locals dictionary, with the exception data. */
	locals = PyDict_New();
	PyDict_SetItemString(locals, "exc_type", ptype);
	PyDict_SetItemString(locals, "exc_value", pvalue);
	PyDict_SetItemString(locals, "exc_traceback", ptraceback);

	/* Run the Python code. */
	ret = PyRun_String(err_handle, Py_file_input, globals, locals);

	Py_DECREF(globals);
	Py_DECREF(locals);
	Py_XDECREF(ret);
}

/**
 * Outputs the compiled bytecode for a given python file, using in-memory
 * caching of bytecode. */
static PyCodeObject *compilePython(char *filename)
{
	shstr *sh_path = NULL;
	struct stat stat_buf;
	struct _node *n;
	int i;
	cacheentry *entry_replace = NULL, *run = NULL;

	if (stat(filename, &stat_buf))
	{
		LOG(llevDebug, "Python: The script file %s can't be stat()ed.\n", filename);
		return NULL;
	}

	FREE_AND_COPY_HASH(sh_path, filename);

	/* Search through cache. Three cases:
	 * 1) script in cache, but older than file  -> replace cached
	 * 2) script in cache and up to date        -> use cached
	 * 3) script not in cache, cache not full   -> add to end of cache
	 * 4) script not in cache, cache full       -> replace least recently used */
	for (i = 0; i < PYTHON_CACHE_SIZE; i++)
	{
		/* Script not in cache, cache not full */
		if (python_cache[i].file == NULL)
		{
			/* Add to end of cache. */
			entry_replace = &python_cache[i];
			break;
		}
		else if (python_cache[i].file == sh_path)
		{
			/* Script in cache */
			if (python_cache[i].code == NULL || (python_cache[i].cached_time < stat_buf.st_mtime))
			{
				/* Cache older than file, replace cached. */
				entry_replace = &python_cache[i];
			}
			else
			{
				/* cache up-to-date, use cached*/
				entry_replace = NULL;
				run = &python_cache[i];
			}

			break;
		}
		else if (entry_replace == NULL || python_cache[i].used_time < entry_replace->used_time)
		{
			/* If we haven't found it yet, set replace to the oldest cache */
			entry_replace = &python_cache[i];
		}
	}

	/* Replace a specific cache index with the file. */
	if (entry_replace)
	{
		FILE *fp;

		fp = fopen(filename, "r");

		if (!fp)
		{
			LOG(llevDebug, "Python: The script file %s can't be opened.\n", filename);
			FREE_AND_CLEAR_HASH(sh_path);
			return NULL;
		}

		Py_XDECREF(entry_replace->code);
		entry_replace->code = NULL;

		/* Need to replace path string? */
		if (entry_replace->file != sh_path)
		{
			if (entry_replace->file)
			{
				FREE_AND_CLEAR_HASH(entry_replace->file);
			}

			FREE_AND_COPY_HASH(entry_replace->file, sh_path);
		}

#ifdef WIN32
		{
			char buf[HUGE_BUF], *pystr = NULL;
			size_t buf_len = 0, pystr_len = 0;

			while (fgets(buf, sizeof(buf), fp))
			{
				buf_len = strlen(buf);
				pystr_len += buf_len;
				pystr = realloc(pystr, sizeof(char) * (pystr_len + 1));
				strcpy(pystr + pystr_len - buf_len, buf);
				pystr[pystr_len] = '\0';
			}

			n = PyParser_SimpleParseString(pystr, Py_file_input);
			free(pystr);
		}
#else
		n = PyParser_SimpleParseFile(fp, filename, Py_file_input);
#endif

		if (n)
		{
			entry_replace->code = PyNode_Compile(n, filename);
			PyNode_Free(n);
		}

		if (PyErr_Occurred())
		{
			PyErr_LOG();
		}
		else
		{
			entry_replace->cached_time = stat_buf.st_mtime;
			entry_replace->used_time = time(NULL);
		}

		fclose(fp);
		run = entry_replace;
	}

	FREE_AND_CLEAR_HASH(sh_path);

	if (run)
	{
		return run->code;
	}

	return NULL;
}

static int do_script(PythonContext *context, const char *filename, object *event)
{
	PyCodeObject *pycode;
	PyObject *dict, *ret;
	char path[HUGE_BUF];

	strncpy(path, hooks->create_pathname(filename), sizeof(path) - 1);

	if (event && path[0] != '/')
	{
		char tmp_path[HUGE_BUF];
		object *outermost = event;

		while (outermost && outermost->env)
		{
			outermost = outermost->env;
		}

		if (outermost && outermost->map)
		{
			hooks->normalize_path(outermost->map->path, filename, tmp_path);
			strncpy(path, hooks->create_pathname(tmp_path), sizeof(path) - 1);
		}
	}

	pycode = compilePython(path);

	if (pycode)
	{
#ifndef PRODUCTION_SERVER
		PyObject *modules = PyImport_GetModuleDict(), *key, *value;
		Py_ssize_t pos = 0;
		const char *m_filename;
		char m_buf[MAX_BUF];

		/* Create path name to the Python scripts directory. */
		strncpy(m_buf, hooks->create_pathname("/python"), sizeof(m_buf) - 1);

		/* Go through the loaded modules. */
		while (PyDict_Next(modules, &pos, &key, &value))
		{
			m_filename = PyModule_GetFilename(value);

			if (!m_filename)
			{
				PyErr_Clear();
				continue;
			}

			/* If this module was loaded from one of our script files,
			 * reload it. */
			if (!strncmp(m_filename, m_buf, strlen(m_buf)))
			{
				PyImport_ReloadModule(value);

				if (PyErr_Occurred())
				{
					PyErr_LOG();
				}
			}
		}
#endif

		pushContext(context);
		dict = PyDict_New();
		PyDict_SetItemString(dict, "__builtins__", PyEval_GetBuiltins());

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 2
		ret = PyEval_EvalCode((PyObject *) pycode, dict, NULL);
#else
		ret = PyEval_EvalCode(pycode, dict, NULL);
#endif

		if (PyErr_Occurred())
		{
			PyErr_LOG();
		}

		Py_XDECREF(ret);
		Py_DECREF(dict);

		return 1;
	}

	return 0;
}

/**
 * Handles normal events.
 * @param args List of arguments for context.
 * @return 0 on failure, script's return value otherwise. */
static int handle_event(va_list args)
{
	char *script;
	PythonContext *context = malloc(sizeof(PythonContext));
	int rv;

	context->activator = va_arg(args, object *);
	context->who = va_arg(args, object *);
	context->other = va_arg(args, object *);
	context->event = va_arg(args, object *);
	context->text = va_arg(args, char *);
	context->parms[0] = va_arg(args, int);
	context->parms[1] = va_arg(args, int);
	context->parms[2] = va_arg(args, int);
	context->parms[3] = va_arg(args, int);
	script = va_arg(args, char *);
	context->options = va_arg(args, char *);
	context->returnvalue = 0;

	LOG(llevDebug, "Python: Start script file >%s<, activator: %s, who: %s, other: %s, text: %s, parms: %d, %d, %d, %d\n", script, STRING_OBJ_NAME(context->activator), STRING_OBJ_NAME(context->who), STRING_OBJ_NAME(context->other), STRING_SAFE(context->text), context->parms[0], context->parms[1], context->parms[2], context->parms[3]);

	if (!do_script(context, script, context->who))
	{
		freeContext(context);
		return 0;
	}

	context = popContext();

	if (context->parms[3] == SCRIPT_FIX_ALL)
	{
		if (context->other && IS_LIVE(context->other))
		{
			hooks->fix_player(context->other);
		}

		if (context->who && IS_LIVE(context->who))
		{
			hooks->fix_player(context->who);
		}

		if (context->activator && IS_LIVE(context->activator))
		{
			hooks->fix_player(context->activator);
		}
	}
	else if (context->parms[3] == SCRIPT_FIX_ACTIVATOR && IS_LIVE(context->activator))
	{
		hooks->fix_player(context->activator);
	}

	rv = context->returnvalue;
	freeContext(context);
	LOG(llevDebug, "Python: done (returned: %d).\n", rv);

	return rv;
}

/**
 * Handles map events.
 * @param args List of arguments for context.
 * @return 0 on failure, script's return value otherwise. */
static int handle_map_event(va_list args)
{
	PythonContext *context = calloc(1, sizeof(PythonContext));
	char *script;
	int rv;

	context->activator = va_arg(args, object *);
	context->event = va_arg(args, object *);
	context->other = va_arg(args, object *);
	context->who = va_arg(args, object *);
	script = va_arg(args, char *);
	context->options = va_arg(args, char *);
	context->text = va_arg(args, char *);
	context->parms[0] = va_arg(args, int);

	if (!do_script(context, script, context->who))
	{
		freeContext(context);
		return 0;
	}

	context = popContext();
	rv = context->returnvalue;
	freeContext(context);

	return rv;
}

/**
 * Handles global event.
 * @param event_type The event ID.
 * @param args List of arguments for context.
 * @return 0. */
static int handle_global_event(int event_type, va_list args)
{
	PythonContext *context;

	switch (event_type)
	{
		case GEVENT_CACHE_REMOVED:
		{
			void *ptr = va_arg(args, void *);
			uint32 flags = *(uint32 *) va_arg(args, void *);

			if (flags & CACHE_FLAG_PYOBJ)
			{
				PyObject *retval;

				/* Attempt to close file/database/etc objects. */
				retval = PyObject_CallMethod((PyObject *) ptr, "close", "");

				/* No close() method, ignore the exception. */
				if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_AttributeError))
				{
					PyErr_Clear();
				}

				Py_XDECREF(retval);

				/* Decrease the reference count. */
				Py_DECREF((PyObject *) ptr);
			}

			return 0;
		}
	}

	context = malloc(sizeof(PythonContext));
	context->activator = NULL;
	context->who = NULL;
	context->other = NULL;
	context->event = NULL;
	context->parms[0] = 0;
	context->parms[1] = 0;
	context->parms[2] = 0;
	context->parms[3] = 0;
	context->text = NULL;
	context->options = NULL;
	context->returnvalue = 0;

	switch (event_type)
	{
		case GEVENT_BORN:
			context->activator = (object *) va_arg(args, void *);
			break;

		case GEVENT_LOGIN:
			context->activator = ((player *) va_arg(args, void *))->ob;
			context->text = (char *) va_arg(args, void *);
			break;

		case GEVENT_LOGOUT:
			context->activator = ((player *) va_arg(args, void *))->ob;
			context->text = (char *) va_arg(args, void *);
			break;

		case GEVENT_PLAYER_DEATH:
			break;
	}

	if (!do_script(context, "/python/events/python_event.py", NULL))
	{
		freeContext(context);
		return 0;
	}

	context = popContext();
	freeContext(context);

	return 0;
}

MODULEAPI void *triggerEvent(int *type, ...)
{
	va_list args;
	int eventcode, event_type;
	static int result = 0;

	va_start(args, type);
	event_type = va_arg(args, int);
	eventcode = va_arg(args, int);

	LOG(llevDebug, "Python: triggerEvent(): eventcode %d\n", eventcode);

	switch (event_type)
	{
		case PLUGIN_EVENT_NORMAL:
			result = handle_event(args);
			break;

		case PLUGIN_EVENT_MAP:
			result = handle_map_event(args);
			break;

		case PLUGIN_EVENT_GLOBAL:
			result = handle_global_event(eventcode, args);
			break;

		default:
			LOG(llevBug, "Python: Requested unknown event type %d.\n", event_type);
			break;
	}

	va_end(args);
	return &result;
}

MODULEAPI void *getPluginProperty(int *type, ...)
{
	va_list args;
	const char *propname;
	int i, size;
	char *buf;

	va_start(args, type);
	propname = va_arg(args, const char *);

	if (!strcmp(propname, "command?"))
	{
		const char *cmdname = va_arg(args, const char *);
		CommArray_s *rtn_cmd = va_arg(args, CommArray_s *);

		va_end(args);

		for (i = 0; i < NR_CUSTOM_CMD; i++)
		{
			if (CustomCommand[i].name != NULL)
			{
				if (!strcmp(CustomCommand[i].name, cmdname))
				{
					rtn_cmd->name = CustomCommand[i].name;
					rtn_cmd->time = (float) CustomCommand[i].speed;
					rtn_cmd->func = cmd_customPython;
					rtn_cmd->flags = 0;
					NextCustomCommand = i;
					return rtn_cmd;
				}
			}
		}

		return NULL;
	}
	else if (!strcmp(propname, "Identification"))
	{
		buf = va_arg(args, char *);
		size = va_arg(args, int);
		va_end(args);
		snprintf(buf, size, PLUGIN_NAME);
		return NULL;
	}
	else if (!strcmp(propname, "FullName"))
	{
		buf = va_arg(args, char *);
		size = va_arg(args, int);
		va_end(args);
		snprintf(buf, size, PLUGIN_VERSION);
		return NULL;
	}

	va_end(args);
	return NULL;
}

/**
 * Run custom command using Python script.
 * @param op Object running the command.
 * @param params Command parameters.
 * @return 0 on failure, return value of the script otherwise. */
static int cmd_customPython(object *op, char *params)
{
	PythonContext *context = malloc(sizeof(PythonContext));
	int rv;

	LOG(llevDebug, "Python: handling command %s using script: %s\n", CustomCommand[NextCustomCommand].name, CustomCommand[NextCustomCommand].script);

	context->activator = op;
	context->who = op;
	context->other = op;
	context->event = NULL;
	context->parms[0] = 0;
	context->parms[1] = 0;
	context->parms[2] = 0;
	context->parms[3] = 0;
	context->text = params;
	context->options = NULL;
	context->returnvalue = 0;

	if (!do_script(context, CustomCommand[NextCustomCommand].script, NULL))
	{
		freeContext(context);
		return 0;
	}

	context = popContext();
	rv = context->returnvalue;
	freeContext(context);

	LOG(llevDebug, "Python: done (returned: %d).\n", rv);

	return rv;
}

MODULEAPI void postinitPlugin()
{
	char path[HUGE_BUF];
	FILE *fp;

	LOG(llevDebug, "Python: Start postinitPlugin.\n");
	hooks->register_global_event(PLUGIN_NAME, GEVENT_CACHE_REMOVED);
	initContextStack();

	strncpy(path, hooks->create_pathname("/python/events/python_init.py"), sizeof(path) - 1);
	fp = fopen(path, "r");

	if (fp)
	{
#ifdef WIN32
		char *pystring;

		fclose(fp);

		pystring = malloc(strlen(path) + 64);
		sprintf(pystring, "exec(open('%s').read())", path);
		PyRun_SimpleString(pystring);
		free(pystring);
#else
		PyRun_SimpleFile(fp, path);
		fclose(fp);
#endif
	}
}

#ifdef IS_PY3K
static PyModuleDef AtrinikModule =
{
	PyModuleDef_HEAD_INIT,
	"Atrinik",
	NULL,
	-1,
	AtrinikMethods,
	NULL, NULL, NULL, NULL
};

static PyObject *PyInit_Atrinik()
{
	PyObject *m = PyModule_Create(&AtrinikModule);
	Py_INCREF(m);
	return m;
}
#endif

/**
 * Create a module.
 * @param name Name of the module.
 * @return The new module created using PyModule_New(). */
static PyObject *module_create(const char *name)
{
	char tmp[MAX_BUF];

	snprintf(tmp, sizeof(tmp), "Atrinik_%s", name);

	return PyModule_New(tmp);
}

/**
 * Creates a new module containing integer constants, and adds it to the
 * specified module.
 * @param module Module to add to.
 * @param name Name of the created module.
 * @param constants Constants to add. */
static void module_add_constants(PyObject *module, const char *name, const Atrinik_Constant *consts)
{
	size_t i = 0;
	PyObject *module_tmp;

	/* Create the new module. */
	module_tmp = module_create(name);

	/* Append constants. */
	while (consts[i].name)
	{
		PyModule_AddIntConstant(module_tmp, consts[i].name, consts[i].value);
		i++;
	}

	/* Add the module. */
	PyDict_SetItemString(PyModule_GetDict(module), name, module_tmp);
}

/**
 * Construct a list from C array and add it to the specified module.
 * @param module Module to add to.
 * @param name Name of the list.
 * @param array Pointer to the C array.
 * @param array_size Number of entries in the C array.
 * @param type Type of the entries in the C array. */
static void module_add_array(PyObject *module, const char *name, void *array, size_t array_size, field_type type)
{
	size_t i;
	PyObject *list;

	/* Create a new list. */
	list = PyList_New(0);

	/* Add entries to the list. */
	for (i = 0; i < array_size; i++)
	{
		if (type == FIELDTYPE_SINT32)
		{
			PyList_Append(list, Py_BuildValue("i", ((sint32 *) array)[i]));
		}
		else if (type == FIELDTYPE_CSTR)
		{
			PyList_Append(list, Py_BuildValue("s", ((char **) array)[i]));
		}
	}

	/* Add it to the module dictionary. */
	PyDict_SetItemString(PyModule_GetDict(module), name, list);
}

/**
 * Open a log file in replacement for stdout and stderr.
 * @param fp File pointer.
 * @param name Name, for example, \<stdout\>.
 * @return The opened log file. */
static PyObject *python_openlogfile(FILE *fp, char *name)
{
#ifdef IS_PY3K
	return PyFile_FromFd(fileno(fp), name, "w", 1, NULL, NULL, NULL, 0);
#else
	return PyFile_FromFile(fp, name, "w", 0);
#endif
}

MODULEAPI void initPlugin(struct plugin_hooklist *hooklist)
{
	PyObject *m, *d, *module_tmp, *logfile_ptr;
	int i;

	hooks = hooklist;

	LOG(llevDebug, "Python: Atrinik Python Plugin loading...\n");

#ifdef IS_PY26
	Py_Py3kWarningFlag++;
#endif

#ifdef IS_PY3K
	PyImport_AppendInittab("Atrinik", &PyInit_Atrinik);
#endif

	Py_Initialize();

	LOG(llevDebug, "Python: Start initAtrinik.\n");

#ifdef IS_PY3K
	m = PyImport_ImportModule("Atrinik");
#else
	m = Py_InitModule("Atrinik", AtrinikMethods);
#endif

	d = PyModule_GetDict(m);
	AtrinikError = PyErr_NewException("Atrinik.error", NULL, NULL);
	PyDict_SetItemString(d, "AtrinikError", AtrinikError);

	for (i = 0; i < NR_CUSTOM_CMD; i++)
	{
		CustomCommand[i].name = NULL;
		CustomCommand[i].script = NULL;
		CustomCommand[i].speed = 0.0;
	}

	if (!Atrinik_Object_init(m) || !Atrinik_Map_init(m) || !Atrinik_Party_init(m) || !Atrinik_Region_init(m) || !Atrinik_Player_init(m) || !Atrinik_Archetype_init(m) || !Atrinik_AttrList_init(m))
	{
		return;
	}

	logfile_ptr = python_openlogfile(*hooks->logfile, "<stdout>");
	PySys_SetObject("stdout", logfile_ptr);
	PySys_SetObject("__stdout__", logfile_ptr);
	PySys_SetObject("stderr", logfile_ptr);
	PySys_SetObject("__stderr__", logfile_ptr);

	module_add_constants(m, "Type", constants_types);
	module_add_array(m, "freearr_x", hooks->freearr_x, SIZEOFFREE, FIELDTYPE_SINT32);
	module_add_array(m, "freearr_y", hooks->freearr_y, SIZEOFFREE, FIELDTYPE_SINT32);

	/* Initialize integer constants */
	for (i = 0; constants[i].name; i++)
	{
		PyModule_AddIntConstant(m, constants[i].name, constants[i].value);
	}

	/* Initialize integer constants */
	for (i = 0; constants_colors[i][0]; i++)
	{
		PyModule_AddStringConstant(m, constants_colors[i][0], constants_colors[i][1]);
	}

	module_tmp = module_create("Gender");
	module_add_array(module_tmp, "gender_noun", hooks->gender_noun, GENDER_MAX, FIELDTYPE_CSTR);
	module_add_array(module_tmp, "gender_subjective", hooks->gender_subjective, GENDER_MAX, FIELDTYPE_CSTR);
	module_add_array(module_tmp, "gender_subjective_upper", hooks->gender_subjective_upper, GENDER_MAX, FIELDTYPE_CSTR);
	module_add_array(module_tmp, "gender_objective", hooks->gender_objective, GENDER_MAX, FIELDTYPE_CSTR);
	module_add_array(module_tmp, "gender_possessive", hooks->gender_possessive, GENDER_MAX, FIELDTYPE_CSTR);
	module_add_array(module_tmp, "gender_reflexive", hooks->gender_reflexive, GENDER_MAX, FIELDTYPE_CSTR);

	for (i = 0; constants_gender[i].name; i++)
	{
		PyModule_AddIntConstant(module_tmp, constants_gender[i].name, constants_gender[i].value);
	}

	PyDict_SetItemString(d, "Gender", module_tmp);

	LOG(llevDebug, "Python:  [Done]\n");
}

MODULEAPI void closePlugin()
{
	LOG(llevDebug, "Python Plugin closing.\n");
	hooks->cache_remove_by_flags(CACHE_FLAG_GEVENT);
	Py_Finalize();
}

/**
 * Sets face field.
 * @param ptr Pointer to ::New_Face structure.
 * @param face_id ID of the face to set.
 * @return 0 on success, -1 on failure. */
static int set_face_field(void *ptr, long face_id)
{
	if (face_id < 0 || face_id >= *hooks->nrofpixmaps)
	{
		PyErr_Format(PyExc_ValueError, "Illegal value for face field: %ld", face_id);
		return -1;
	}

	*(New_Face **) ptr = &(*hooks->new_faces)[face_id];
	return 0;
}

/**
 * Sets animation field.
 * @param ptr Pointer to ::uint16 structure member.
 * @param anim_id ID of the animation to set.
 * @return 0 on success, -1 on failure. */
static int set_animation_field(void *ptr, long anim_id)
{
	if (anim_id < 0 || anim_id >= *hooks->num_animations)
	{
		PyErr_Format(PyExc_ValueError, "Illegal value for animation field: %ld", anim_id);
		return -1;
	}

	*(uint16 *) ptr = (uint16) anim_id;
	return 0;
}

/**
 * A generic field setter for all interfaces.
 * @param type Type of the field.
 * @param[out] field_ptr Field pointer.
 * @param value Value to set for the field pointer.
 * @return 0 on success, -1 on failure. */
int generic_field_setter(fields_struct *field, void *ptr, PyObject *value)
{
	void *field_ptr;

	if ((field->flags & FIELDFLAG_READONLY))
	{
		INTRAISE("Trying to modify readonly field.");
	}

	field_ptr = (void *) ((char *) ptr + field->offset);

	switch (field->type)
	{
		case FIELDTYPE_SHSTR:
			if (value == Py_None)
			{
				FREE_AND_CLEAR_HASH(*(shstr **) field_ptr);
			}
			else if (PyString_Check(value))
			{
				FREE_AND_CLEAR_HASH(*(shstr **) field_ptr);
				FREE_AND_COPY_HASH(*(shstr **) field_ptr, PyString_AsString(value));
			}
			else
			{
				INTRAISE("Illegal value for shared string field.");
			}

			break;

		case FIELDTYPE_CSTR:
			if (value == Py_None)
			{
				FREE_AND_NULL_PTR(*(char **) field_ptr);
			}
			else if (PyString_Check(value))
			{
				if (*(char **) field_ptr)
				{
					free(*(char **) field_ptr);
				}

				*(char **) field_ptr = hooks->strdup_local(PyString_AsString(value));
			}
			else
			{
				INTRAISE("Illegal value for C string field.");
			}

			break;

		case FIELDTYPE_CARY:
			if (value == Py_None)
			{
				((char *) field_ptr)[0] = '\0';
			}
			else if (PyString_Check(value))
			{
				memcpy((char *) field_ptr, PyString_AsString(value), field->extra_data);
				((char *) field_ptr)[field->extra_data] = '\0';
			}
			else
			{
				INTRAISE("Illegal value for C char array field.");
			}

			break;

		case FIELDTYPE_UINT8:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < 0 || (unsigned long) val > UINT8_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for uint8 field.");
					return -1;
				}

				*(uint8 *) field_ptr = (uint8) val;
			}
			else
			{
				INTRAISE("Illegal value for uint8 field.");
			}

			break;

		case FIELDTYPE_SINT8:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < SINT8_MIN || val > SINT8_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for sint8 field.");
					return -1;
				}

				*(sint8 *) field_ptr = (sint8) val;
			}
			else
			{
				INTRAISE("Illegal value for sint8 field.");
			}

			break;

		case FIELDTYPE_UINT16:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < 0 || (unsigned long) val > UINT16_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for uint16 field.");
					return -1;
				}

				*(uint16 *) field_ptr = (uint16) val;
			}
			else
			{
				INTRAISE("Illegal value for uint16 field.");
			}

			break;

		case FIELDTYPE_SINT16:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < SINT16_MIN || val > SINT16_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for sint16 field.");
					return -1;
				}

				*(sint16 *) field_ptr = (sint16) val;
			}
			else
			{
				INTRAISE("Illegal value for sint16 field.");
			}

			break;

		case FIELDTYPE_UINT32:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < 0 || (unsigned long) val > UINT32_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for uint32 field.");
					return -1;
				}

				*(uint32 *) field_ptr = (uint32) val;
			}
			else
			{
				INTRAISE("Illegal value for uint32 field.");
			}

			break;

		case FIELDTYPE_SINT32:
			if (PyInt_Check(value))
			{
				long val = PyLong_AsLong(value);

				if (val < SINT32_MIN || val > SINT32_MAX)
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for sint32 field.");
					return -1;
				}

				*(sint32 *) field_ptr = (sint32) val;
			}
			else
			{
				INTRAISE("Illegal value for sint32 field.");
			}

			break;

		case FIELDTYPE_UINT64:
			if (PyInt_Check(value))
			{
				unsigned PY_LONG_LONG val = PyLong_AsUnsignedLongLong(value);

				if (PyErr_Occurred())
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for uint64 field.");
					return -1;
				}

				*(uint64 *) field_ptr = (uint64) val;
			}
			else
			{
				INTRAISE("Illegal value for uint64 field.");
			}

			break;

		case FIELDTYPE_SINT64:
			if (PyInt_Check(value))
			{
				PY_LONG_LONG val = PyLong_AsLongLong(value);

				if (PyErr_Occurred())
				{
					PyErr_SetString(PyExc_OverflowError, "Invalid integer value for sint64 field.");
					return -1;
				}

				*(sint64 *) field_ptr = (sint64) val;
			}
			else
			{
				INTRAISE("Illegal value for sint64 field.");
			}

			break;

		case FIELDTYPE_FLOAT:
			if (PyFloat_Check(value))
			{
				*(float *) field_ptr = PyFloat_AsDouble(value) * 1.0;
			}
			else if (PyInt_Check(value))
			{
				*(float *) field_ptr = PyLong_AsLong(value) * 1.0;
			}
			else
			{
				INTRAISE("Illegal value for float field.");
			}

			break;

		case FIELDTYPE_OBJECT:
			if (value == Py_None)
			{
				*(object **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_ObjectType))
			{
				OBJEXISTCHECK_INT((Atrinik_Object *) value);
				*(object **) field_ptr = (object *) ((Atrinik_Object *) value)->obj;
			}
			else
			{
				INTRAISE("Illegal value for object field.");
			}

			break;

		case FIELDTYPE_OBJECT2:
			INTRAISE("Field type not implemented.");
			break;

		case FIELDTYPE_MAP:
			if (value == Py_None)
			{
				*(mapstruct **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_MapType))
			{
				*(mapstruct **) field_ptr = (mapstruct *) ((Atrinik_Map *) value)->map;
			}
			else
			{
				INTRAISE("Illegal value for map field.");
			}

			break;

		case FIELDTYPE_OBJECTREF:
		{
			void *field_ptr2 = (void *) ((char *) ptr + field->extra_data);

			if (value == Py_None)
			{
				*(object **) field_ptr = NULL;
				*(tag_t *) field_ptr2 = 0;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_ObjectType))
			{
				object *tmp;

				OBJEXISTCHECK_INT((Atrinik_Object *) value);

				tmp = (object *) ((Atrinik_Object *) value)->obj;
				*(object **) field_ptr = tmp;
				*(tag_t *) field_ptr2 = tmp->count;
			}
			else
			{
				INTRAISE("Illegal value for object+reference field.");
			}

			break;
		}

		case FIELDTYPE_REGION:
			if (value == Py_None)
			{
				*(region **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_RegionType))
			{
				*(region **) field_ptr = (region *) ((Atrinik_Region *) value)->region;
			}
			else
			{
				INTRAISE("Illegal value for region field.");
			}

			break;

		case FIELDTYPE_PARTY:
			if (value == Py_None)
			{
				*(party_struct **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_PartyType))
			{
				*(party_struct **) field_ptr = (party_struct *) ((Atrinik_Party *) value)->party;
			}
			else
			{
				INTRAISE("Illegal value for party field.");
			}

			break;

		case FIELDTYPE_ARCH:
			if (value == Py_None)
			{
				*(archetype **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_ArchetypeType))
			{
				*(archetype **) field_ptr = (archetype *) ((Atrinik_Archetype *) value)->at;
			}
			else
			{
				INTRAISE("Illegal value for archetype field.");
			}

			break;

		case FIELDTYPE_PLAYER:
			if (value == Py_None)
			{
				*(player **) field_ptr = NULL;
			}
			else if (PyObject_TypeCheck(value, &Atrinik_PlayerType))
			{
				*(player **) field_ptr = (player *) ((Atrinik_Player *) value)->pl;
			}
			else
			{
				INTRAISE("Illegal value for player field.");
			}

			break;

		case FIELDTYPE_FACE:
			if (PyTuple_Check(value))
			{
				if (PyTuple_GET_SIZE(value) != 2)
				{
					PyErr_Format(PyExc_ValueError, "Tuple for face field must have exactly two values.");
					return -1;
				}
				else if (!PyInt_Check(PyTuple_GET_ITEM(value, 1)))
				{
					PyErr_SetString(PyExc_ValueError, "Second value of tuple used for face field is not an integer.");
					return -1;
				}

				return set_face_field(field_ptr, PyLong_AsLong(PyTuple_GET_ITEM(value, 1)));
			}
			else if (PyInt_Check(value))
			{
				return set_face_field(field_ptr, PyLong_AsLong(value));
			}
			else if (PyString_Check(value))
			{
				return set_face_field(field_ptr, hooks->find_face(PyString_AsString(value), 0));
			}
			else
			{
				INTRAISE("Illegal value for face field.");
			}

			break;

		case FIELDTYPE_ANIMATION:
			if (PyTuple_Check(value))
			{
				if (PyTuple_GET_SIZE(value) != 2)
				{
					PyErr_Format(PyExc_ValueError, "Tuple for animation field must have exactly two values.");
					return -1;
				}
				else if (!PyInt_Check(PyTuple_GET_ITEM(value, 1)))
				{
					PyErr_SetString(PyExc_ValueError, "Second value of tuple used for animation field is not an integer.");
					return -1;
				}

				return set_animation_field(field_ptr, PyLong_AsLong(PyTuple_GET_ITEM(value, 1)));
			}
			else if (PyInt_Check(value))
			{
				return set_animation_field(field_ptr, PyLong_AsLong(value));
			}
			else if (PyString_Check(value))
			{
				return set_animation_field(field_ptr, hooks->find_animation(PyString_AsString(value)));
			}
			else
			{
				INTRAISE("Illegal value for animation field.");
			}

			break;

		case FIELDTYPE_BOOLEAN:
			if (value == Py_True)
			{
				*(uint8 *) field_ptr = 1;
			}
			else if (value == Py_False)
			{
				*(uint8 *) field_ptr = 0;
			}
			else
			{
				INTRAISE("Illegal value for boolean field.");
			}

			break;

		default:
			break;
	}

	return 0;
}

/**
 * A generic field getter for all interfaces.
 * @param type Type of the field.
 * @param field_ptr Field pointer.
 * @param field_ptr2 Field pointer for extra data.
 * @return Python object containing value of field_ptr (and field_ptr2, if applicable). */
PyObject *generic_field_getter(fields_struct *field, void *ptr)
{
	void *field_ptr;

	field_ptr = (void *) ((char *) ptr + field->offset);

	switch (field->type)
	{
		case FIELDTYPE_SHSTR:
		case FIELDTYPE_CSTR:
		{
			char *str = *(char **) field_ptr;
			return Py_BuildValue("s", str ? str : "");
		}

		case FIELDTYPE_CARY:
			return Py_BuildValue("s", (char *) field_ptr);

		case FIELDTYPE_UINT8:
			return Py_BuildValue("B", *(uint8 *) field_ptr);

		case FIELDTYPE_SINT8:
			return Py_BuildValue("b", *(sint8 *) field_ptr);

		case FIELDTYPE_UINT16:
			return Py_BuildValue("H", *(uint16 *) field_ptr);

		case FIELDTYPE_SINT16:
			return Py_BuildValue("h", *(sint16 *) field_ptr);

		case FIELDTYPE_UINT32:
			return Py_BuildValue("I", *(uint32 *) field_ptr);

		case FIELDTYPE_SINT32:
			return Py_BuildValue("i", *(sint32 *) field_ptr);

		case FIELDTYPE_UINT64:
			return Py_BuildValue("K", *(uint64 *) field_ptr);

		case FIELDTYPE_SINT64:
			return Py_BuildValue("L", *(sint64 *) field_ptr);

		case FIELDTYPE_FLOAT:
			return Py_BuildValue("f", *(float *) field_ptr);

		case FIELDTYPE_MAP:
			return wrap_map(*(mapstruct **) field_ptr);

		case FIELDTYPE_OBJECT:
			return wrap_object(*(object **) field_ptr);

		case FIELDTYPE_OBJECT2:
			return wrap_object((object *) field_ptr);

		case FIELDTYPE_OBJECTREF:
		{
			object *obj = *(object **) field_ptr;
			tag_t tag = *(tag_t *) (void *) ((char *) ptr + field->extra_data);

			return wrap_object(OBJECT_VALID(obj, tag) ? obj : NULL);
		}

		case FIELDTYPE_REGION:
			return wrap_region(*(region **) field_ptr);

		case FIELDTYPE_PARTY:
			return wrap_party(*(party_struct **) field_ptr);

		case FIELDTYPE_ARCH:
			return wrap_archetype(*(archetype **) field_ptr);

		case FIELDTYPE_PLAYER:
			return wrap_player(*(player **) field_ptr);

		case FIELDTYPE_FACE:
			return Py_BuildValue("(sH)", (*(New_Face **) field_ptr)->name, (*(New_Face **) field_ptr)->number);

		case FIELDTYPE_ANIMATION:
			return Py_BuildValue("(sH)", (&(*hooks->animations)[*(uint16 *) field_ptr])->name, *(uint16 *) field_ptr);

		case FIELDTYPE_BOOLEAN:
			Py_ReturnBoolean(*(uint8 *) field_ptr);

		case FIELDTYPE_LIST:
			return wrap_attr_list(ptr, field->offset, field->extra_data);

		default:
			break;
	}

	RAISE("Unknown field type.");
}

/**
 * Generic rich comparison function.
 * @param op
 * @param result
 * @return  */
PyObject *generic_rich_compare(int op, int result)
{
	/* Based on how Python 3.0 (GPL compatible) implements it for internal types. */
	switch (op)
	{
		case Py_EQ:
			result = (result == 0);
			break;
		case Py_NE:
			result = (result != 0);
			break;
		case Py_LE:
			result = (result <= 0);
			break;
		case Py_GE:
			result = (result >= 0);
			break;
		case Py_LT:
			result = (result == -1);
			break;
		case Py_GT:
			result = (result == 1);
			break;
	}

	return PyBool_FromLong(result);
}

/**
 * Call a function defined in Python script with the specified arguments.
 * @param callable What to call.
 * @param arglist Arguments to call the function with. Will have reference
 * decreased.
 * @return Integer value the function returned. */
int python_call_int(PyObject *callable, PyObject *arglist)
{
	PyObject *result;
	int retval = 0;

	/* Call the Python function. */
	result = PyEval_CallObject(callable, arglist);

	/* Check the result. */
	if (result && PyInt_Check(result))
	{
		retval = PyInt_AsLong(result);
	}

	Py_XDECREF(result);
	Py_DECREF(arglist);

	return retval;
}
