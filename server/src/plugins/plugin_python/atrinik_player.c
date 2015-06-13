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
 * Atrinik Python plugin player related code. */

#include <plugin_python.h>
#include <packet.h>
#include <faction.h>

/**
 * Player fields. */
/* @cparser
 * @page plugin_python_player_fields Python player fields
 * <h2>Python player fields</h2>
 * List of the player fields and their meaning. */
static fields_struct fields[] = {
    {"next", FIELDTYPE_PLAYER, offsetof(player, next), FIELDFLAG_READONLY, 0,
            "Next player in a list.; Atrinik.Player.Player or None (readonly)"},
    {"prev", FIELDTYPE_PLAYER, offsetof(player, prev), FIELDFLAG_READONLY, 0,
            "Previous player in a list.; Atrinik.Player.Player or None "
            "(readonly)"},

    {"party", FIELDTYPE_PARTY, offsetof(player, party), FIELDFLAG_READONLY, 0,
            "Party the player is a member of.; Atrinik.Party.Party or None "
            "(readonly)"},
    {"class_ob", FIELDTYPE_OBJECT, offsetof(player, class_ob), 0,
            FIELDFLAG_READONLY, "Class object of the player. Cannot be set, as "
            "it's always set to the last CLASS type object that is found in "
            "the player's inventory after calling "
            ":meth:`Atrinik.Object.Object.Update`.; Atrinik.ObjectObject or "
            "None (readonly)"},
    {"savebed_map", FIELDTYPE_CARY, offsetof(player, savebed_map), 0,
            sizeof(((player *) NULL)->savebed_map),
            "Path to the player's savebed map.; str"},
    {"bed_x", FIELDTYPE_INT16, offsetof(player, bed_x), 0, 0,
            "X coordinate of the player's savebed.; int"},
    {"bed_y", FIELDTYPE_INT16, offsetof(player, bed_y), 0, 0,
            "Y coordinate of the player's savebed.; int"},
    {"ob", FIELDTYPE_OBJECT, offsetof(player, ob), FIELDFLAG_READONLY, 0,
            "The :class:`Atrinik.Object.Object` representing the player.; "
            "Atrinik.Object.Object (readonly)"},
    {"quest_container", FIELDTYPE_OBJECT, offsetof(player, quest_container),
            FIELDFLAG_READONLY, 0, "Player's quest container.; "
            "Atrinik.Object.Object (readonly)"},
    {"target_object", FIELDTYPE_OBJECTREF, offsetof(player, target_object), 0,
            offsetof(player, target_object_count), "Currently targeted "
            "NPC/monster.;Atrinik.Object.Object or None"},
    {"no_chat", FIELDTYPE_BOOLEAN, offsetof(player, no_chat), 0, 0,
            "If true, the player is not allowed to chat.; bool"},
    {"tcl", FIELDTYPE_BOOLEAN, offsetof(player, tcl), 0, 0,
            "If true, the player ignores collision with terrain.; bool"},
    {"tgm", FIELDTYPE_BOOLEAN, offsetof(player, tgm), 0, 0,
            "If true, the player is in god-mode and cannot die or take damage "
            "from any source.; bool"},
    {"tli", FIELDTYPE_BOOLEAN, offsetof(player, tli), 0, 0,
            "If true, the player has lighting disabled.; bool"},
    {"tls", FIELDTYPE_BOOLEAN, offsetof(player, tls), 0, 0,
            "If true, the player ignores line of sight.; bool"},
    {"tsi", FIELDTYPE_BOOLEAN, offsetof(player, tsi), 0, 0,
            "If true, the player can see system-invisible objects.; bool"},
    {"cmd_permissions", FIELDTYPE_LIST, offsetof(player, cmd_permissions), 0,
            FIELDTYPE_CMD_PERMISSIONS, "Player's command permissions.; "
            "Atrinik.AttrList.AttrList"},
    {"factions", FIELDTYPE_LIST, offsetof(player, factions), 0,
            FIELDTYPE_FACTIONS, "Player's factions.; "
            "Atrinik.AttrList.AttrList"},
    {"fame", FIELDTYPE_INT64, offsetof(player, fame), 0, 0,
            "Fame (or infamy) of the player.; int"},
    {"container", FIELDTYPE_OBJECT, offsetof(player, container),
            FIELDFLAG_READONLY, 0, "Container the player has open.; "
            "Atrinik.Object.Object or None (readonly)"},

    {"s_ext_title_flag", FIELDTYPE_BOOLEAN,
            offsetof(player, socket.ext_title_flag), 0, 0,
            "If True, will force updating the player's map name.; bool"},
    {"s_host", FIELDTYPE_CSTR, offsetof(player, socket.host),
            FIELDFLAG_READONLY, 0, "IP address of the player.; str (readonly)"},
    {"s_socket_version", FIELDTYPE_UINT32,
            offsetof(player, socket.socket_version), FIELDFLAG_READONLY, 0,
            "Socket version of the player's client.; int (readonly)"}
};
/* @endcparser */

/**
 * @defgroup plugin_python_player_functions Python player functions
 * Functions that can only be used on players (not the objects they are
 * controlling).
 *
 * To access object's player controller, you can use something like:
 *
 * @code
 * activator = WhoIsActivator()
 * player = activator.Controller()
 * @endcode
 *
 * In the above example, player points to the player structure (which Python
 * is wrapping) that is controlling the object 'activator'. In this way, you
 * can,
 * for example, use something like this to get player's save bed, among other
 * things:
 *
 * @code
 * print(WhoIsActivator().Controller().savebed_map)
 * @endcode
 *@{*/

/** Documentation for Atrinik_Player_GetEquipment(). */
static const char doc_Atrinik_Player_GetEquipment[] =
".. method:: GetEquipment(slot).\n\n"
"Get player's current equipment object for a given slot.\n\n"
":param slot: The slot number; one of PLAYER_EQUIP_xxx.\n"
":type slot: int\n"
":returns: The equipment for the given slot, None if there's no object in the "
"slot.\n"
":rtype: :class:`Atrinik.Object.Object` or None";

/**
 * Implements Atrinik.Player.GetEquipment() Python method.
 * @copydoc PyMethod_VARARGS
 */
static PyObject *Atrinik_Player_GetEquipment(Atrinik_Player *pl, PyObject *args)
{
    int slot;

    if (!PyArg_ParseTuple(args, "i", &slot)) {
        return NULL;
    }

    if (slot < 0 || slot >= PLAYER_EQUIP_MAX) {
        PyErr_SetString(PyExc_ValueError, "Invalid slot number.");
        return NULL;
    }

    return wrap_object(pl->pl->equipment[slot]);
}

/**
 * <h1>player.CanCarry(object|int what)</h1>
 * Check whether the player can carry the object 'what', taking weight limit
 * into consideration.
 * @param what Object that player wants to get. This can be the exact weight
 * to check instead of calculating the object's weight.
 * @throws ValueError if 'what' is neither an object nor an integer.
 * @return True if the player can carry the object, False otherwise. */
static PyObject *Atrinik_Player_CanCarry(Atrinik_Player *pl, PyObject *what)
{
    uint32_t weight;

    if (PyObject_TypeCheck(what, &Atrinik_ObjectType)) {
        OBJEXISTCHECK((Atrinik_Object *) what);
        weight = WEIGHT_NROF(((Atrinik_Object *) what)->obj, ((Atrinik_Object *) what)->obj->nrof);
    } else if (PyInt_Check(what)) {
        weight = PyInt_AsLong(what);
    } else {
        PyErr_SetString(PyExc_ValueError, "Invalid value for 'what' parameter.");
        return NULL;
    }

    Py_ReturnBoolean(hooks->player_can_carry(pl->pl->ob, weight));
}

/**
 * <h1>player.AddExp(int|string skill, int exp, int [exact = False], int [level = True])</h1>
 * Add (or subtract) experience.
 * @param skill ID or name of the skill to receive/lose exp in.
 * @param exp How much exp to gain/lose. If 'level' is true, this is the number
 * of levels to gain/lose in the specified skill.
 * @param exact If True, the given exp will not be capped.
 * @param level If True, will calculate exact experience needed for next (or
 * previous) level. */
static PyObject *Atrinik_Player_AddExp(Atrinik_Player *pl, PyObject *args)
{
    PyObject *skill;
    uint32_t skill_nr;
    int64_t exp_gain;
    int exact = 0, level = 0;

    if (!PyArg_ParseTuple(args, "OL|ii", &skill, &exp_gain, &exact, &level)) {
        return NULL;
    }

    if (PyInt_Check(skill)) {
        skill_nr = PyInt_AsLong(skill);

        if (skill_nr >= NROFSKILLS) {
            PyErr_Format(PyExc_ValueError, "Skill ID '%d' is invalid; 0-%d should be used.", skill_nr, NROFSKILLS - 1);
            return NULL;
        }
    } else if (PyString_Check(skill)) {
        const char *skill_name;

        skill_name = PyString_AsString(skill);

        for (skill_nr = 0; skill_nr < NROFSKILLS; skill_nr++) {
            if (strcmp(hooks->skills[skill_nr].name, skill_name) == 0) {
                break;
            }
        }

        if (skill_nr == NROFSKILLS) {
            PyErr_Format(PyExc_ValueError, "Skill '%s' does not exist.", skill_name);
            return NULL;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid object type for 'skill' parameter.");
        return NULL;
    }

    if (pl->pl->skill_ptr[skill_nr] == NULL) {
        PyErr_Format(AtrinikError, "Player %s does not have the skill '%s'.", pl->pl->ob->name, hooks->skills[skill_nr].name);
        return NULL;
    }

    if (level != 0) {
        level = MAX(1, MIN(MAXLEVEL, pl->pl->skill_ptr[skill_nr]->level + exp_gain));
        exp_gain = hooks->level_exp(level, 1.0) - pl->pl->skill_ptr[skill_nr]->stats.exp;
    }

    hooks->add_exp(pl->pl->ob, exp_gain, skill_nr, exact);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.BankDeposit(string text)</h1>
 * Deposit money to bank.
 * @param text How much money to deposit, in string representation.
 * @return Tuple containing the status code (one of @ref BANK_xxx) and amount
 * of money deposited as integer. */
static PyObject *Atrinik_Player_BankDeposit(Atrinik_Player *pl, PyObject *args)
{
    const char *text;
    int ret;
    int64_t value;

    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }

    ret = hooks->bank_deposit(pl->pl->ob, text, &value);

    return Py_BuildValue("(iL)", ret, value);
}

/**
 * <h1>player.BankWithdraw(string text)</h1>
 * Withdraw money from bank.
 * @param text How much money to withdraw, in string representation.
 * @return Tuple containing the status code (one of @ref BANK_xxx) and amount
 * of money withdrawn as integer. */
static PyObject *Atrinik_Player_BankWithdraw(Atrinik_Player *pl, PyObject *args)
{
    const char *text;
    int ret;
    int64_t value;

    if (!PyArg_ParseTuple(args, "s", &text)) {
        return NULL;
    }

    ret = hooks->bank_withdraw(pl->pl->ob, text, &value);

    return Py_BuildValue("(iL)", ret, value);
}

/**
 * <h1>player.BankBalance()</h1>
 * Figure out how much money player has in bank.
 * @return Integer value of the money in bank. */
static PyObject *Atrinik_Player_BankBalance(Atrinik_Player *pl, PyObject *args)
{
    (void) args;

    return Py_BuildValue("L", hooks->bank_get_balance(pl->pl->ob));
}

/**
 * <h1>player.SwapApartments(string oldmap, string newmap, int x, int y)</h1>
 * Swaps oldmap apartment with newmap one.
 *
 * Copies old items from oldmap to newmap at x, y and saves the map.
 * @param oldmap The old apartment map.
 * @param oldmap The new apartment map.
 * @param x X position to copy the items to.
 * @param y Y position to copy the items to.
 * @return True on success, False on failure. */
static PyObject *Atrinik_Player_SwapApartments(Atrinik_Player *pl, PyObject *args)
{
    const char *mapold, *mapnew;
    int x, y;

    if (!PyArg_ParseTuple(args, "ssii", &mapold, &mapnew, &x, &y)) {
        return NULL;
    }

    Py_ReturnBoolean(hooks->swap_apartments(mapold, mapnew, x, y, pl->pl->ob));
}

/**
 * <h1>player.ExecuteCommand(string command)</h1>
 * Make player execute a command.
 * @param command Command to execute.
 * @throws AtrinikError if player is not in a state to execute commands.
 * @return Return value of the command. */
static PyObject *Atrinik_Player_ExecuteCommand(Atrinik_Player *pl, PyObject *args)
{
    const char *command;
    char *cp;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }

    if (pl->pl->socket.state != ST_PLAYING) {
        PyErr_SetString(AtrinikError, "Player is not in a state to execute commands.");
        return NULL;
    }

    /* Make a copy of the command, since execute_newserver_command
     * modifies the string. */
    cp = strdup(command);
    hooks->commands_handle(pl->pl->ob, cp);
    free(cp);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.FindMarkedObject()</h1>
 * Find marked object in player's inventory.
 * @return The marked object, or None if no object is marked. */
static PyObject *Atrinik_Player_FindMarkedObject(Atrinik_Player *pl, PyObject *args)
{
    (void) args;

    return wrap_object(hooks->find_marked_object(pl->pl->ob));
}

/**
 * <h1>player.Sound(string filename, int [type = @ref CMD_SOUND_EFFECT], int [x
 * = 0], int [y = 0], int [loop = 0], int [volume = 0])</h1>
 * Play a sound to the specified player.
 * @param filename Sound file to play.
 * @param type Sound type being played, one of @ref CMD_SOUND_xxx.
 * @param x X position where the sound is playing from. Can be 0.
 * @param y Y position where the sound is playing from. Can be 0.
 * @param loop How many times to loop the sound, -1 for infinite number.
 * @param volume Volume adjustment. */
static PyObject *Atrinik_Player_Sound(Atrinik_Player *pl, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"filename", "type", "x", "y", "loop", "volume", NULL};
    const char *filename;
    int type = CMD_SOUND_EFFECT, x = 0, y = 0, loop = 0, volume = 0;

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|iiiii", kwlist, &filename, &type, &x, &y, &loop, &volume)) {
        return NULL;
    }

    hooks->play_sound_player_only(pl->pl, type, filename, x, y, loop, volume);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.Examine(object obj, bool [ret = False])</h1>
 * Makes player examine the specified object.
 * @param obj Object to examine. */
static PyObject *Atrinik_Player_Examine(Atrinik_Player *pl, PyObject *args)
{
    Atrinik_Object *obj;
    int ret = 0;
    StringBuffer *sb_capture = NULL;

    if (!PyArg_ParseTuple(args, "O!|i", &Atrinik_ObjectType, &obj, &ret)) {
        return NULL;
    }

    if (ret) {
        sb_capture = hooks->stringbuffer_new();
    }

    hooks->examine(pl->pl->ob, obj->obj, sb_capture);

    if (ret) {
        char *cp;
        PyObject *retval;

        cp = hooks->stringbuffer_finish(sb_capture);
        retval = Py_BuildValue("s", cp);
        efree(cp);

        return retval;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.SendPacket(int command, string format, ...)</h1>
 * Constructs and sends a packet to the player's client.
 * @param command The command ID.
 * @param format Format specifier. For example, "Bs" would imply uint8 +
 * string data, and the format specifier would need to be followed by an
 * integer that is within uint8 data range and a string. */
static PyObject *Atrinik_Player_SendPacket(Atrinik_Player *pl, PyObject *args)
{
    long cmd;
    char *format;
    packet_struct *packet;
    size_t i;
    PyObject *value;

    /* Must have at least 3 arguments. */
    if (PyTuple_Size(args) < 3) {
        PyErr_SetString(PyExc_ValueError, "Insufficient number of arguments.");
        return NULL;
    }

    /* The first argument must be an integer. */
    if (!PyInt_Check(PyTuple_GET_ITEM(args, 0))) {
        PyErr_SetString(PyExc_OverflowError, "Illegal value for 'cmd' function argument.");
        return NULL;
    }

    cmd = PyLong_AsLong(PyTuple_GET_ITEM(args, 0));

    /* It also must be uint8. */
    if (cmd < 0 || (unsigned long) cmd > UINT8_MAX) {
        PyErr_SetString(PyExc_OverflowError, "Invalid integer value for 'cmd' function argument.");
        return NULL;
    }

    packet = hooks->packet_new(cmd, 256, 512);

    /* Get the format specifier. */
    format = PyString_AsString(PyTuple_GET_ITEM(args, 1));

    for (i = 0; format[i] != '\0'; i++) {
        value = PyTuple_GetItem(args, 2 + i);

        if (!value) {
            PyErr_SetString(PyExc_ValueError, "Insufficient number of arguments.");
            return NULL;
        }

        if (format[i] == 'b') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < INT8_MIN || val > INT8_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_int8(packet, val);
                continue;
            }
        } else if (format[i] == 'B') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < 0 || (unsigned long) val > UINT8_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_uint8(packet, val);
                continue;
            }
        } else if (format[i] == 'h') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < INT16_MIN || val > INT16_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_int16(packet, val);
                continue;
            }
        } else if (format[i] == 'H') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < 0 || (unsigned long) val > UINT16_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_uint16(packet, val);
                continue;
            }
        } else if (format[i] == 'i') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < INT32_MIN || val > INT32_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_int32(packet, val);
                continue;
            }
        } else if (format[i] == 'I') {
            if (PyInt_Check(value)) {
                long val = PyLong_AsLong(value);

                if (val < 0 || (unsigned long) val > UINT32_MAX) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_uint32(packet, val);
                continue;
            }
        } else if (format[i] == 'l') {
            if (PyInt_Check(value)) {
                PY_LONG_LONG val = PyLong_AsLongLong(value);

                if (PyErr_Occurred()) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_int64(packet, val);
                continue;
            }
        } else if (format[i] == 'L') {
            if (PyInt_Check(value)) {
                unsigned PY_LONG_LONG val = PyLong_AsUnsignedLongLong(value);

                if (PyErr_Occurred()) {
                    PyErr_Format(PyExc_OverflowError, "Invalid integer value for '%c' format specifier.", format[i]);
                    return NULL;
                }

                hooks->packet_append_uint64(packet, val);
                continue;
            }
        } else if (format[i] == 's') {
            if (PyString_Check(value)) {
                hooks->packet_append_string_terminated(packet, PyString_AsString(value));
                continue;
            }
        } else if (format[i] == 'x') {
            if (PyBytes_Check(value)) {
                hooks->packet_append_data_len(packet, (uint8_t *) PyBytes_AsString(value), PyBytes_Size(value));
                continue;
            }
        } else {
            PyErr_Format(PyExc_TypeError, "Illegal format specifier '%c'.", format[i]);
            return NULL;
        }

        PyErr_Format(PyExc_TypeError, "Illegal value for '%c' format specifier.", format[i]);
        return NULL;
    }

    hooks->socket_send_packet(&pl->pl->socket, packet);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.DrawInfo(string message, string [color = @ref COLOR_ORANGE], int
 * [type = @ref CHAT_TYPE_GAME], bool [global = False], string [name =
 * None])</h1>
 * Sends a message to the specified player.
 * @param message The message to send.
 * @param color Color to use for the message. Can be one of @ref COLOR_xxx
 * or an HTML color notation.
 * @param type One of @ref CHAT_TYPE_xxx.
 * @param global If True, the message will be broadcasted to all players.
 * @param name Player name that is the source of this message, if applicable.
 * If None and 'type' is not @ref CHAT_TYPE_GAME, player.ob.name will be used.
 */
static PyObject *Atrinik_Player_DrawInfo(Atrinik_Player *pl, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"message", "color", "type", "global", "name", NULL};
    const char *message, *color, *name;
    uint8_t type, global;

    color = COLOR_ORANGE;
    type = CHAT_TYPE_GAME;
    global = 0;
    name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|sbbz", kwlist, &message, &color, &type, &global, &name)) {
        return NULL;
    }

    if (!name && type != CHAT_TYPE_GAME) {
        name = pl->pl->ob->name;
    }

    hooks->draw_info_type(type, name, color, global ? NULL : pl->pl->ob, message);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.FactionGetBounty(string faction)</h1>
 * Acquires player's bounty for the specified faction.
 * @param faction The faction name.
 * @return Player's bounty in the specified faction.
 */
static PyObject *Atrinik_Player_FactionGetBounty(Atrinik_Player *pl, PyObject *args)
{
    const char *name;
    shstr *sh_name;
    faction_t faction;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    sh_name = hooks->find_string(name);

    if (sh_name == NULL) {
        PyErr_Format(AtrinikError, "No such faction: %s", name);
        return NULL;
    }

    faction = hooks->faction_find(sh_name);

    if (faction == NULL) {
        PyErr_Format(AtrinikError, "No such faction: %s", name);
        return NULL;
    }

    return Py_BuildValue("d", hooks->faction_get_bounty(faction, pl->pl));
}

/**
 * <h1>player.FactionClearBounty(string faction)</h1>
 * Clear player's bounty for the specified faction.
 * @param faction The faction name.
 */
static PyObject *Atrinik_Player_FactionClearBounty(Atrinik_Player *pl, PyObject *args)
{
    const char *name;
    shstr *sh_name;
    faction_t faction;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    sh_name = hooks->find_string(name);

    if (sh_name == NULL) {
        PyErr_Format(AtrinikError, "No such faction: %s", name);
        return NULL;
    }

    faction = hooks->faction_find(sh_name);

    if (faction == NULL) {
        PyErr_Format(AtrinikError, "No such faction: %s", name);
        return NULL;
    }

    hooks->faction_clear_bounty(faction, pl->pl);

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 * <h1>player.InsertCoins(int value)</h1>
 * Gives coins of the specified value to the player.
 * @param value The value.
 */
static PyObject *Atrinik_Player_InsertCoins(Atrinik_Player *pl, PyObject *args)
{
    int64_t value;

    if (!PyArg_ParseTuple(args, "l", &value)) {
        return NULL;
    }

    hooks->insert_coins(pl->pl->ob, value);

    Py_INCREF(Py_None);
    return Py_None;
}

/*@}*/

/** Available Python methods for the AtrinikPlayer type. */
static PyMethodDef methods[] = {
    {"GetEquipment", (PyCFunction) Atrinik_Player_GetEquipment, METH_VARARGS,
            doc_Atrinik_Player_GetEquipment},
    {"CanCarry", (PyCFunction) Atrinik_Player_CanCarry, METH_O, 0},
    {"AddExp", (PyCFunction) Atrinik_Player_AddExp, METH_VARARGS, 0},
    {"BankDeposit", (PyCFunction) Atrinik_Player_BankDeposit, METH_VARARGS, 0},
    {"BankWithdraw", (PyCFunction) Atrinik_Player_BankWithdraw, METH_VARARGS, 0},
    {"BankBalance", (PyCFunction) Atrinik_Player_BankBalance, METH_NOARGS, 0},
    {"SwapApartments", (PyCFunction) Atrinik_Player_SwapApartments, METH_VARARGS, 0},
    {"ExecuteCommand", (PyCFunction) Atrinik_Player_ExecuteCommand, METH_VARARGS, 0},
    {"FindMarkedObject", (PyCFunction) Atrinik_Player_FindMarkedObject, METH_NOARGS, 0},
    {"Sound", (PyCFunction) Atrinik_Player_Sound, METH_VARARGS | METH_KEYWORDS, 0},
    {"Examine", (PyCFunction) Atrinik_Player_Examine, METH_VARARGS, 0},
    {"SendPacket", (PyCFunction) Atrinik_Player_SendPacket, METH_VARARGS, 0},
    {"DrawInfo", (PyCFunction) Atrinik_Player_DrawInfo, METH_VARARGS | METH_KEYWORDS, 0},
    {"FactionGetBounty", (PyCFunction) Atrinik_Player_FactionGetBounty, METH_VARARGS, 0},
    {"FactionClearBounty", (PyCFunction) Atrinik_Player_FactionClearBounty, METH_VARARGS, 0},
    {"InsertCoins", (PyCFunction) Atrinik_Player_InsertCoins, METH_VARARGS, 0},
    {NULL, NULL, 0, 0}
};

/**
 * Get player's attribute.
 * @param pl Python player wrapper.
 * @param context Void pointer to the field ID.
 * @return Python object with the attribute value, NULL on failure. */
static PyObject *get_attribute(Atrinik_Player *pl, void *context)
{
    return generic_field_getter(context, pl->pl);
}

/**
 * Set attribute of a player.
 * @param whoptr Python player wrapper.
 * @param value Value to set.
 * @param context Void pointer to the field.
 * @return 0 on success, -1 on failure. */
static int set_attribute(Atrinik_Player *pl, PyObject *value, void *context)
{
    fields_struct *field = context;

    if (generic_field_setter(field, pl->pl, value) == -1) {
        return -1;
    }

    if (field->offset == offsetof(player, target_object)) {
        hooks->send_target_command(pl->pl);
    }

    return 0;
}

/**
 * Create a new player wrapper.
 * @param type Type object.
 * @param args Unused.
 * @param kwds Unused.
 * @return The new wrapper. */
static PyObject *Atrinik_Player_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Atrinik_Player *pl;

    (void) args;
    (void) kwds;

    pl = (Atrinik_Player *) type->tp_alloc(type, 0);

    if (pl) {
        pl->pl = NULL;
    }

    return (PyObject *) pl;
}

/**
 * Free a player wrapper.
 * @param pl The wrapper to free. */
static void Atrinik_Player_dealloc(Atrinik_Player *pl)
{
    pl->pl = NULL;
#ifndef IS_PY_LEGACY
    Py_TYPE(pl)->tp_free((PyObject *) pl);
#else
    pl->ob_type->tp_free((PyObject *) pl);
#endif
}

/**
 * Return a string representation of a player.
 * @param pl The player.
 * @return Python object containing the name of the player. */
static PyObject *Atrinik_Player_str(Atrinik_Player *pl)
{
    return Py_BuildValue("s", pl->pl->ob->name);
}

static int Atrinik_Player_InternalCompare(Atrinik_Player *left, Atrinik_Player *right)
{
    return (left->pl < right->pl ? -1 : (left->pl == right->pl ? 0 : 1));
}

static PyObject *Atrinik_Player_RichCompare(Atrinik_Player *left, Atrinik_Player *right, int op)
{
    if (!left || !right || !PyObject_TypeCheck((PyObject *) left, &Atrinik_PlayerType) || !PyObject_TypeCheck((PyObject *) right, &Atrinik_PlayerType)) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    return generic_rich_compare(op, Atrinik_Player_InternalCompare(left, right));
}

/**
 * This is filled in when we initialize our player type. */
static PyGetSetDef getseters[NUM_FIELDS + 1];

/** Our actual Python PlayerType. */
PyTypeObject Atrinik_PlayerType = {
#ifdef IS_PY3K
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,
#endif
    "Atrinik.Player",
    sizeof(Atrinik_Player),
    0,
    (destructor) Atrinik_Player_dealloc,
    NULL, NULL, NULL,
#ifdef IS_PY3K
    NULL,
#else
    (cmpfunc) Atrinik_Player_InternalCompare,
#endif
    0, 0, 0, 0, 0, 0,
    (reprfunc) Atrinik_Player_str,
    0, 0, 0,
    Py_TPFLAGS_DEFAULT,
    "Atrinik players",
    NULL, NULL,
    (richcmpfunc) Atrinik_Player_RichCompare,
    0, 0, 0,
    methods,
    0,
    getseters,
    0, 0, 0, 0, 0, 0, 0,
    Atrinik_Player_new,
    0, 0, 0, 0, 0, 0, 0, 0
#ifndef IS_PY_LEGACY
    , 0
#endif
#ifdef Py_TPFLAGS_HAVE_FINALIZE
    , NULL
#endif
};

/**
 * Initialize the player wrapper.
 * @param module The Atrinik Python module.
 * @return 1 on success, 0 on failure. */
int Atrinik_Player_init(PyObject *module)
{
    size_t i;

    /* Field getters */
    for (i = 0; i < NUM_FIELDS; i++) {
        PyGetSetDef *def = &getseters[i];

        def->name = fields[i].name;
        def->get = (getter) get_attribute;
        def->set = (setter) set_attribute;
        def->doc = fields[i].doc;
        def->closure = &fields[i];
    }

    getseters[i].name = NULL;

    Atrinik_PlayerType.tp_new = PyType_GenericNew;

    if (PyType_Ready(&Atrinik_PlayerType) < 0) {
        return 0;
    }

    Py_INCREF(&Atrinik_PlayerType);
    PyModule_AddObject(module, "Player", (PyObject *) & Atrinik_PlayerType);

    return 1;
}

/**
 * Utility method to wrap a player.
 * @param what Player to wrap.
 * @return Python object wrapping the real player. */
PyObject *wrap_player(player *pl)
{
    Atrinik_Player *wrapper;

    /* Return None if no player was to be wrapped. */
    if (!pl) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    wrapper = PyObject_NEW(Atrinik_Player, &Atrinik_PlayerType);

    if (wrapper) {
        wrapper->pl = pl;
    }

    return (PyObject *) wrapper;
}
