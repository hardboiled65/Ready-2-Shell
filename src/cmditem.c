/*
//  cmditem.c
//
//  Author:     Pale Purple
//  Created:    2016. 02. 10. 21:35
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#include "cmditem.h"

#include <stdlib.h>
#include <string.h>

void cmditem_init(struct cmditem *cmditem)
{
    cmditem->parent = NULL;
    cmditem->left = NULL;
    cmditem->right = NULL;
    cmditem->cmd = NULL;
    cmditem->prio = 0;
    cmditem->desc = NULL;
    cmditem->line = 0;
}

int cmditem_parse_string(struct cmditem *cmditem, const char *str)
{
    int count;
    int i;

    /* count and set cmd */
    count = 0;
    for (i = 0; (str[i] != ' ') && (str[i] != '\t'); ++i) {
        count += 1;
    }
    if (count == 0) {
        return CMDITEM_ERROR_PARSE_ERROR;
    }
    cmditem->cmd = (char*)malloc((sizeof(char) * count) + 1);
    strncpy(cmditem->cmd, str, count + 1);
    cmditem->cmd[count] = '\0';
    /* discard spaces or tabs */
    for (; (str[i] == ' ') || (str[i] == '\t'); ++i) {
    }
    /* set priority */
    if ( (cmditem->prio = cmditem_to_prio(str[i++])) == -1 ) {
        return CMDITEM_ERROR_PARSE_ERROR;
    }
    /* discard spaces or tabs */
    for (; (str[i] == ' ') || (str[i] == '\t'); ++i) {
        if (str[i] == '\0') {
            break;
        }
    }
    /* count and set desc */
    if (str[i] != '\0') {
        count = 0;
        for (; str[i] != '\0'; ++i) {
            count += 1;
        }
        cmditem->desc = (char*)malloc((sizeof(char) * count) + 1);
        strncpy(cmditem->desc, str + (i - count), count + 1);
        cmditem->desc[count] = '\0';
    } else { /* no description */
    }

    return CMDITEM_ERROR_NO_ERROR;
}

int cmditem_append(struct cmditem *cmditem, struct cmditem *new_item)
{
    struct cmditem *it;
    struct cmditem *past_it;

    if (cmditem->parent != NULL) {
        /* parent of root is null */
        return CMDITEM_ERROR_NOT_ROOT;
    }

    past_it = cmditem;
    it = cmditem;
    while (it != NULL) {
        past_it = it;
        it = (strcmp(new_item->cmd, it->cmd) < 0) ? it->left : it->right;
    }
    if (strcmp(new_item->cmd, past_it->cmd) < 0) {
        past_it->left = new_item;
    } else {
        past_it->right = new_item;
    }
    new_item->parent = past_it;

    return CMDITEM_ERROR_NO_ERROR;
}

struct cmditem* cmditem_find(struct cmditem *cmditem, const char *key)
{
    struct cmditem *it;
    struct cmditem *find = NULL;

    it = cmditem;
    /* return immediately if root is not set (means empty tree) */
    if (it->cmd == NULL) {
        return find;
    }
    while (it != NULL) {
        if (strcmp(key, it->cmd) < 0) {
            it = it->left;
        } else if (strcmp(key, it->cmd) > 0) {
            it = it->right;
        } else {
            find = it;
            break;
        }
    }
    return find;
}

void cmditem_traversal(struct cmditem *cmditem, void (*func)(struct cmditem*))
{
    if (cmditem->left != NULL) {
        cmditem_traversal(cmditem->left, func);
    }
    func(cmditem);
    if (cmditem->right != NULL) {
        cmditem_traversal(cmditem->right, func);
    }
}

struct cmditem* cmditem_begin(struct cmditem *cmditem)
{
    return least_child(cmditem);
}

struct cmditem* cmditem_next(struct cmditem *cmditem, struct cmditem *item)
{
    struct cmditem *next;

    if (item->right != NULL) {
        /* go to least child of right, or to right if not */
        if ((next = least_child(item->right)) == NULL) {
            next = item->right;
        }
    } else {
        next = nearest_greater_ancestor(item);
    }

    return next;
}

void cmditem_free(struct cmditem *cmditem)
{
    if (cmditem->left != NULL) {
        cmditem_free(cmditem->left);
    }
    if (cmditem->right != NULL) {
        cmditem_free(cmditem->right);
    }
    if (cmditem != NULL) {
        if (cmditem->cmd != NULL) {
            free(cmditem->cmd);
        }
        if (cmditem->desc != NULL) {
            free(cmditem->desc);
        }
        /* root item should be freed manually */
        if (cmditem->parent != NULL) {
            free(cmditem);
        }
    }
}

/* private functions */
struct cmditem* least_child(struct cmditem *item)
{
    struct cmditem *it;

    it = item->left;
    /* return null if the item is the least */
    if (it == NULL) {
        return it;
    }

    while (it->left != NULL) {
        it = it->left;
    }
    return it;
}

struct cmditem* greatest_child(struct cmditem *item)
{
    struct cmditem *it;

    it = item->right;
    /* return null if the item is the greatest */
    if (it == NULL) {
        return it;
    }

    while (it->right != NULL) {
        it = it->right;
    }
    return it;
}

struct cmditem* nearest_greater_ancestor(struct cmditem *item)
{
    struct cmditem *it;

    it = item;
    while ((it->parent != NULL) && (it != it->parent->left)) {
        it = it->parent;
    }

    /* parent is greater than child if child is left of parent */
    if ((it->parent != NULL) && (it == it->parent->left)) {
        return it->parent;
    }
    /* if 'it' is root, there's no greater parent than the item */
    if (it->parent == NULL) {
        return NULL;
    }
}

struct cmditem* grandparent(struct cmditem *item)
{
    if (item->parent != NULL) {
        return item->parent->parent;
    } else {
        return NULL;
    }
}

/* non-member functions */
int cmditem_to_prio(const char ch)
{
    int prio;

    switch(ch) {
    case 'n':
    case 'N':
        prio = CMDITEM_NORMAL;
        break;
    case 'i':
    case 'I':
        prio = CMDITEM_IMPORTANT;
        break;
    case 'e':
    case 'E':
        prio = CMDITEM_EXTRA;
        break;
    default:
        prio = -1;
        break;
    }

    return prio;
}
