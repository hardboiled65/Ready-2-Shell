/*
//  cmditem.h
//
//  Author:     Pale Purple
//  Created:    2016. 02. 10. 22:03
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#ifndef _CMDITEM_H
#define _CMDITEM_H

/**
 * struct cmditem - an item about the command
 *
 * - description
 * cmditem is as a binary search tree that automatically sorted by ascending
 * order of command name. null parent means the item is the root of the tree.
 *
 * - members
 * parent       parent of this node
 * left         left child
 * right        right child
 * cmd          command name
 * prio         priority (normal / important / extra)
 * desc         user description
 * line         actual line number in file
 */
struct cmditem {
    struct cmditem *parent;
    struct cmditem *left;
    struct cmditem *right;
    char *cmd;
    int prio;
    char *desc;
    int line;
};

enum cmditem_priority {
    CMDITEM_IMPORTANT = 0,
    CMDITEM_NORMAL = 1,
    CMDITEM_EXTRA = 2
};

enum cmditem_error {
    CMDITEM_ERROR_NO_ERROR = 0,
    CMDITEM_ERROR_PARSE_ERROR = 1,
    CMDITEM_ERROR_NOT_ROOT = 2,
};

void cmditem_init(struct cmditem *cmditem);
int cmditem_parse_string(struct cmditem *cmditem, const char *str);
int cmditem_append(struct cmditem *cmditem, struct cmditem *new_item);
struct cmditem* cmditem_find(struct cmditem *cmditem, const char *key);
void cmditem_traversal(struct cmditem *cmditem, void (*func)(struct cmditem*));
int cmditem_remove(struct cmditem *cmditem, struct cmditem *item);
void cmditem_free(struct cmditem *cmditem);
int cmditem_to_prio(const char ch);

#endif /* _CMDITEM_H */
