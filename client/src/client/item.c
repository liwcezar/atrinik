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
 * Object management. */

#include <global.h>

/** The list of free (unused) objects */
static object *free_objects = NULL;

/**
 * Allocates a new object.
 * @return The object. */
static object *object_new(void)
{
    object *op = calloc(1, sizeof(object));

    if (!op) {
        logger_print(LOG(ERROR), "OOM.");
    }

    return op;
}

/**
 * Allocates a list of objects.
 * @param nrof How many objects to allocate.
 * @return The allocated objects in a list. */
static object *objects_alloc(int nrof)
{
    object *op, *list;
    int i;

    list = op = object_new();

    for (i = 1; i < nrof; i++) {
        op->next = object_new();
        op->next->prev = op;
        op = op->next;
    }

    return list;
}

/**
 * Frees all objects in a list.
 * @param op Start of the list. */
void objects_free(object *op)
{
    object *next;

    while (op) {
        if (op->itype == TYPE_SPELL) {
            spells_remove(op);
        }
        else if (op->itype == TYPE_SKILL) {
            skills_remove(op);
        }

        if (op->inv) {
            objects_free(op->inv);
        }

        next = op->next;
        free(op);
        op = next;
    }
}
/**
 * Find an object inside another object, but not inside inventories.
 * @param op Object to search in.
 * @param tag ID of the object we're looking for.
 * @return Matching object if found, NULL otherwise. */
object *object_find_object_inv(object *op, sint32 tag)
{
    object *tmp;

    for (tmp = op->inv; tmp; tmp = tmp->next) {
        if (tmp->tag == tag) {
            return op;
        }
    }

    return NULL;
}

/**
 * Find an object inside another object by its tag.
 * @param op Object to search in.
 * @param tag ID of the object we're looking for.
 * @return Matching object if found, NULL otherwise. */
object *object_find_object(object *op, sint32 tag)
{
    for (; op; op = op->next) {
        if (op->tag == tag) {
            return op;
        }
        else if (op->inv) {
            object *tmp = object_find_object(op->inv, tag);

            if (tmp) {
                return tmp;
            }
        }
    }

    return NULL;
}

/**
 * Attempts to find an object by its tag, wherever it may be.
 * @param tag Tag to look for.
 * @return Matching object if found, NULL otherwise. */
object *object_find(sint32 tag)
{
    object *op;

    if (tag == 0) {
        return cpl.below;
    }

    if (tag == -1) {
        return cpl.sack;
    }

    /* Below the player. */
    if (cpl.below) {
        op = object_find_object(cpl.below->inv, tag);

        if (op) {
            return op;
        }
    }

    /* Open container. */
    if (cpl.sack) {
        op = object_find_object(cpl.sack->inv, tag);

        if (op) {
            return op;
        }
    }

    /* Last attempt, inside the player. */
    return object_find_object(cpl.ob, tag);
}

/**
 * Remove an object.
 * @param op What to remove. */
void object_remove(object *op)
{
    if (!op || op == cpl.ob || op == cpl.below || op == cpl.sack) {
        return;
    }

    if (op->itype == TYPE_SPELL) {
        spells_remove(op);
    }
    else if (op->itype == TYPE_SKILL) {
        skills_remove(op);
    }
    else if (op->itype == TYPE_FORCE || op->itype == TYPE_POISONING) {
        widget_active_effects_remove(cur_widget[ACTIVE_EFFECTS_ID], op);
    }

    if (op->inv) {
        object_remove_inventory(op);
    }

    if (op->prev) {
        op->prev->next = op->next;
    }
    else {
        op->env->inv = op->next;
    }

    if (op->next) {
        op->next->prev = op->prev;
    }

    /* Add object to the list of free objects. */
    op->next = free_objects;

    if (op->next) {
        op->next->prev = op;
    }

    free_objects = op;

    /* Clear the object so it can be reused. */
    memset((void *) ((char *) op + offsetof(object, prev)), 0, sizeof(object) - offsetof(object, prev));
}

/**
 * Remove all items in object's inventory.
 * @param op The object to remove inventory of. */
void object_remove_inventory(object *op)
{
    if (!op) {
        return;
    }

    while (op->inv) {
        object_remove(op->inv);
    }
}

/**
 * Adds an object to inventory of 'env'.
 * @param env Which object to add to.
 * @param op Object to add.
 * @param bflag If 1, the object will be added to the end of the
 * inventory instead of the start. */
static void object_add(object *env, object *op, int bflag)
{
    object *tmp;

    if (!op) {
        return;
    }

    if (!bflag) {
        op->next = env->inv;

        if (op->next) {
            op->next->prev = op;
        }

        op->prev = NULL;
        env->inv = op;
        op->env = env;
    }
    else {
        for (tmp = env->inv; tmp && tmp->next; tmp = tmp->next) {
        }

        op->next = NULL;
        op->prev = tmp;
        op->env = env;

        if (!tmp) {
            env->inv = op;
        }
        else {
            if (tmp->next) {
                tmp->next->prev = op;
            }

            tmp->next = op;
        }
    }
}

/**
 * Creates a new object and inserts it into 'env'.
 * @param env Which object to insert the created object into. Can be NULL
 * not to insert the created object anywhere.
 * @param tag The object's ID.
 * @param bflag If 1, the object will be added to the end of the
 * inventory instead of the start.
 * @return The created object. */
object *object_create(object *env, sint32 tag, int bflag)
{
    object *op;

    /* Allocate more objects if needed. */
    if (!free_objects) {
        free_objects = objects_alloc(NROF_ITEMS);
    }

    op = free_objects;
    free_objects = free_objects->next;

    if (free_objects) {
        free_objects->prev = NULL;
    }

    op->tag = tag;

    if (env) {
        object_add(env, op, bflag);
    }

    return op;
}

/**
 * Toggle the locked status of an object.
 * @param op Object. */
void toggle_locked(object *op)
{
    packet_struct *packet;

    /* If object is on the ground, don't lock it. */
    if (!op || !op->env || op->env->tag == 0) {
        return;
    }

    packet = packet_new(SERVER_CMD_ITEM_LOCK, 8, 0);
    packet_append_uint32(packet, op->tag);
    socket_send_packet(packet);
}

/**
 * Update the marked object.
 * @param op The object. */
void object_send_mark(object *op)
{
    packet_struct *packet;

    /* If object is on the ground, don't mark it. */
    if (!op || !op->env || op->env->tag == 0) {
        return;
    }

    if (cpl.mark_count == op->tag) {
        cpl.mark_count = -1;
    }
    else {
        cpl.mark_count = op->tag;
    }

    packet = packet_new(SERVER_CMD_ITEM_MARK, 8, 0);
    packet_append_uint32(packet, op->tag);
    socket_send_packet(packet);
}

/**
 * Deinitialize the various objects of ::cpl structure. */
void objects_deinit(void)
{
    objects_free(cpl.sack);
    objects_free(cpl.below);
    objects_free(cpl.ob);
}

/**
 * Initializes the various objects of ::cpl structure. */
void objects_init(void)
{
    cpl.ob = object_new();
    cpl.below = object_new();
    cpl.sack = object_new();

    cpl.below->weight = -111;
    cpl.sack->weight = -111;
}

/**
 * Animate one object.
 * @param ob The object to animate. */
static void animate_object(object *ob)
{
    if (ob->animation_id > 0) {
        check_animation_status(ob->animation_id);
    }

    if (ob->animation_id > 0 && ob->anim_speed) {
        ob->last_anim++;

        if (ob->last_anim >= ob->anim_speed) {
            if (++ob->anim_state >= animations[ob->animation_id].frame) {
                ob->anim_state = 0;
            }

            if (ob->direction > animations[ob->animation_id].facings) {
                ob->face = animations[ob->animation_id].faces[ob->anim_state];
            }
            else {
                ob->face = animations[ob->animation_id].faces[animations[ob->animation_id].frame * ob->direction + ob->anim_state];
            }

            ob->last_anim = 0;
        }
    }
}

/**
 * Animate all possible objects. */
void animate_objects(void)
{
    object *ob;

    if (cpl.ob) {
        /* For now, only the players inventory needs to be animated */
        for (ob = cpl.ob->inv; ob; ob = ob->next) {
            animate_object(ob);
        }
    }

    if (cpl.below) {
        for (ob = cpl.below->inv; ob; ob = ob->next) {
            animate_object(ob);
        }
    }

    if (cpl.sack) {
        for (ob = cpl.sack->inv; ob; ob = ob->next) {
            animate_object(ob);
        }
    }
}

/**
 * Draw the object, centering it. Animation offsets are taken into
 * account for perfect centering, even with different image sizes in
 * animation.
 * @param tmp Object to show.
 * @param x X position.
 * @param y Y position. */
void object_show_centered(SDL_Surface *surface, object *tmp, int x, int y)
{
    int temp, xstart, xlen, ystart, ylen;
    sint16 face;
    SDL_Rect box;

    if (!FaceList[tmp->face].sprite) {
        return;
    }

    /* Will be used for coordinate calculations. */
    face = tmp->face;

    /* If the item is animated, try to use the first animation face for
    * coordinate calculations to prevent 'jumping' of the animation. */
    if (tmp->animation_id > 0) {
        check_animation_status(tmp->animation_id);

        if (animations[tmp->animation_id].num_animations && animations[tmp->animation_id].facings <= 1 && FaceList[animations[tmp->animation_id].faces[0]].sprite) {
            face = animations[tmp->animation_id].faces[0];
        }
    }

    xstart = FaceList[face].sprite->border_left;
    xlen = FaceList[face].sprite->bitmap->w - xstart - FaceList[face].sprite->border_right;
    ystart = FaceList[face].sprite->border_up;
    ylen = FaceList[face].sprite->bitmap->h - ystart - FaceList[face].sprite->border_down;

    if (xlen > INVENTORY_ICON_SIZE) {
        box.w = INVENTORY_ICON_SIZE;
        temp = (xlen - INVENTORY_ICON_SIZE) / 2;
        box.x = xstart + temp;
        xstart = 0;
    }
    else {
        box.w = xlen;
        box.x = xstart;
        xstart = (INVENTORY_ICON_SIZE - xlen) / 2;
    }

    if (ylen > INVENTORY_ICON_SIZE) {
        box.h = INVENTORY_ICON_SIZE;
        temp = (ylen - INVENTORY_ICON_SIZE) / 2;
        box.y = ystart + temp;
        ystart = 0;
    }
    else {
        box.h = ylen;
        box.y = ystart;
        ystart = (INVENTORY_ICON_SIZE - ylen) / 2;
    }

    if (face != tmp->face) {
        temp = xstart - box.x;

        box.x = 0;
        box.w = FaceList[tmp->face].sprite->bitmap->w;
        xstart = temp;

        temp = ystart - box.y + (FaceList[face].sprite->bitmap->h - FaceList[tmp->face].sprite->bitmap->h);
        box.y = 0;
        box.h = FaceList[tmp->face].sprite->bitmap->h;
        ystart = temp;

        if (xstart < 0) {
            box.x = -xstart;
            box.w = FaceList[tmp->face].sprite->bitmap->w + xstart;

            if (box.w > INVENTORY_ICON_SIZE) {
                box.w = INVENTORY_ICON_SIZE;
            }

            xstart = 0;
        }
        else {
            if (box.w + xstart > INVENTORY_ICON_SIZE) {
                box.w -= ((box.w + xstart) - INVENTORY_ICON_SIZE);
            }
        }

        if (ystart < 0) {
            box.y = -ystart;
            box.h = FaceList[tmp->face].sprite->bitmap->h + ystart;

            if (box.h > INVENTORY_ICON_SIZE) {
                box.h = INVENTORY_ICON_SIZE;
            }

            ystart = 0;
        }
        else {
            if (box.h + ystart > INVENTORY_ICON_SIZE) {
                box.h -= ((box.h + ystart) - INVENTORY_ICON_SIZE);
            }
        }
    }

    surface_show(surface, x + xstart, y + ystart, &box, FaceList[tmp->face].sprite->bitmap);
}
