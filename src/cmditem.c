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

#include <stdio.h>
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
    if ( (cmditem->prio = cmditem_ctop(str[i++])) == -1 ) {
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

int cmditem_set_cmd(struct cmditem *cmditem, const char *cmd)
{
    if (cmd == NULL) {
        return CMDITEM_ERROR_INVALID_VALUE;
    }
    if (cmditem->cmd != NULL) {
        free(cmditem->cmd);
    }
    cmditem->cmd = (char*)malloc(sizeof(char) * (strlen(cmd) + 1));
    strcpy(cmditem->cmd, cmd);

    return CMDITEM_ERROR_NO_ERROR;
}

int cmditem_set_prio(struct cmditem *cmditem, const char ch)
{
    int prio;

    prio = cmditem_ctop(ch);
    if (prio == -1) {
        return CMDITEM_ERROR_INVALID_VALUE;
    }
    cmditem->prio = prio;

    return CMDITEM_ERROR_NO_ERROR;
}

int cmditem_set_desc(struct cmditem *cmditem, const char *desc)
{
    if (desc == NULL) {
        return CMDITEM_ERROR_INVALID_VALUE;
    }
    if (cmditem->desc != NULL) {
        free(cmditem->desc);
    }
    cmditem->desc = (char*)malloc(sizeof(char) * (strlen(desc) + 1));
    strcpy(cmditem->desc, desc);

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

    /* the root is empty */
    if (cmditem->cmd == NULL) {
        *cmditem = *new_item;
        free(new_item);
        new_item = cmditem;
        return CMDITEM_ERROR_NO_ERROR;
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
    if (cmditem->cmd != NULL) {
        func(cmditem);
    }
    if (cmditem->right != NULL) {
        cmditem_traversal(cmditem->right, func);
    }
}

struct cmditem* cmditem_begin(struct cmditem *cmditem)
{
    if (cmditem->cmd == NULL) {
        return NULL;
    }

    if (cmditem->left != NULL) {
        return least_child(cmditem);
    } else {
        return cmditem;
    }
}

struct cmditem* cmditem_next(struct cmditem *cmditem)
{
    struct cmditem *next;

    if (cmditem->right != NULL) {
        /* go to least child of right, or to right if not */
        if ((next = least_child(cmditem->right)) == NULL) {
            next = cmditem->right;
        }
    } else {
        next = nearest_greater_ancestor(cmditem);
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
int cmditem_ctop(const char ch)
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
        prio = CMDITEM_NO_PRIO;
        break;
    }

    return prio;
}

int cmditem_stop(const char *str)
{
    int prio;

    switch (atoi(str)) {
    case CMDITEM_IMPORTANT:
        prio = CMDITEM_IMPORTANT;
        break;
    case CMDITEM_NORMAL:
        prio = CMDITEM_NORMAL;
        break;
    case CMDITEM_EXTRA:
        prio = CMDITEM_EXTRA;
        break;
    default:
        prio = CMDITEM_NO_PRIO;
        break;
    }

    return prio;
}

int cmditem_to_prio(const char ch)
{
    printf("cmditem_to_prio() is deprecated. use cmditem_ctop() instead.\n");
    return cmditem_ctop(ch);
}

int cmditem_str_to_prio(const char *str)
{
    printf("cmditem_str_to_prio() is deprecated. use cmditem_stop() instead.\n");
    return cmditem_stop(str);
}

char cmditem_ptoc(int prio)
{
    char ch = 0;

    switch (prio) {
    case CMDITEM_IMPORTANT:
        ch = 'i';
        break;
    case CMDITEM_NORMAL:
        ch = 'n';
        break;
    case CMDITEM_EXTRA:
        ch = 'e';
        break;
    default:
        break;
    }

    return ch;
}
