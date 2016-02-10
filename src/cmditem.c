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
    while (it->left != NULL || it->right != NULL) {
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

void cmditem_free(struct cmditem *cmditem)
{
}
