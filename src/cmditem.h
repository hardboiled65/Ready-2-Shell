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
 * line         actual line number in file. begins from 1
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
    CMDITEM_EXTRA = 2,
    CMDITEM_NO_PRIO = 3,
};

enum cmditem_error {
    CMDITEM_ERROR_NO_ERROR = 0,
    CMDITEM_ERROR_PARSE_ERROR = 1,
    CMDITEM_ERROR_NOT_ROOT = 2,
    CMDITEM_ERROR_INVALID_VALUE = 3,
};

/**
 * void cmditem_init(struct cmditem*) - initiallize cmditem
 */
void cmditem_init(struct cmditem *cmditem);

/**
 * int cmditem_parse_string(struct cmditem*, const char*)
 *
 * - parameters
 * const char* - string to parsing
 *
 * - description
 * receive a string then parse that. cmditem members are set to information
 * from the string
 *
 * - return value
 * CMDITEM_ERROR_NO_ERROR for success, if some problem when parsing, then
 * returns CMDITEM_ERROR_PARSE_ERROR
 */
int cmditem_parse_string(struct cmditem *cmditem, const char *str);

/**
 * int cmditem_set_cmd(struct cmditem *cmditem, const char *cmd)
 *
 * - parameters
 * cmditem - the cmditem to set new cmd
 * cmd - string that source of new cmd
 *
 * - description
 * set cmditem->cmd with given null-terminated string `cmd`. new cmd is dynamic-
 * allocated in the heap. if cmd already set before, it will be replaced with
 * new `cmd` and the old one will be freed
 *
 * - return value
 * CMDITEM_ERROR_NO_ERROR for success, CMDITEM_ERROR_INVALID_VALUE if `cmd` is
 * not a proper string
 */
int cmditem_set_cmd(struct cmditem *cmditem, const char *cmd);

/**
 * int cmditem_set_prio(struct cmditem *cmditem, const char ch)
 *
 * - parameters
 * cmditem - the cmditem to set new prio
 * ch - character that valued 'i', 'n' or 'e'
 *
 * - description
 * set cmditem->prio by refer to `ch`. if `ch` has proper value(see the
 * parameters section), prio is set with matching enum(refer enum
 * cmditem_priority).
 *
 * - return value
 * CMDITEM_ERROR_NO_ERROR for success, CMDITEM_ERROR_INVALID_VALUE if `ch` has
 * an invalid value
 */
int cmditem_set_prio(struct cmditem *cmditem, const char ch);

/**
 * int cmditem_set_desc(struct cmditem *cmditem, const char *desc)
 *
 * - parameters
 * cmditem - the cmditem to set new desc
 * desc - desc string
 *
 * - description
 * set cmditem->desc with `desc` that null-terminated string.
 *
 * - return value
 * CMDITEM_ERROR_NO_ERROR for success, if str is not valid string, then
 * CMDITEM_ERROR_INVALID_VALUE will be returned
 */
int cmditem_set_desc(struct cmditem *cmditem, const char *desc);

/**
 * int cmditem_append(struct cmditem *cmditem, struct cmditem *new_item)
 *
 * -parameters
 * cmditem - the root item of cmditem`s`.
 * new_item - cmditem to appending
 *
 * - description
 * appending new_item to the cmditem inventory. `cmditem` should the root of
 * items. be aware of that if the root is empty, data of `new_item` are copied
 * to root item and `new_item` will be free, then it points the root item.
 *
 * - return value
 * CMDITEM_ERROR_NO_ERROR for no error. if `cmditem` is not the root of items,
 * then returns CMDITEM_ERROR_NOT_ROOT
 */
int cmditem_append(struct cmditem *cmditem, struct cmditem *new_item);

/**
 * struct cmditem* cmditem_find(struct cmditem *cmditem, const char *key)
 *
 * - parameters
 * key - the string to finding
 *
 * - description
 * find the item that has same name in `cmd` member. if `cmditem` not the root,
 * the behavior is unknown
 *
 * - return value
 * pointer to struct cmditem that found. return null if not found or
 * the inventory is empty
 */
struct cmditem* cmditem_find(struct cmditem *cmditem, const char *key);

/**
 * void cmditem_traversal(struct cmditem*, void (*func)(struct cmditem*))
 *
 * - parameters
 * func - a callback function for each cmditem`s`
 *
 * - description
 * travel all items in the cmditem and execute callback function with each
 * items. if cmditem is an empty container(i.e. cmditem->cmd is a null), the
 * function will not executed
 */
void cmditem_traversal(struct cmditem *cmditem, void (*func)(struct cmditem*));

/**
 * int cmditem_remove(struct cmditem*, struct cmditem *item)
 *
 * - parameters
 * item - the pointer to the item to remove
 *
 * - description
 * remove the item from the cmditem.
 *
 * - return value
 * if removing has done successfully, it returns CMDITEM_ERROR_NO_ERROR.
 * else, CMDITEM_ERROR_NO_ITEM will returned if the item not found. and also
 * the cmditem is not the root, CMDITEM_ERROR_NOT_ROOT will returned
 */
int cmditem_remove(struct cmditem *cmditem, struct cmditem *item);

/**
 * struct cmditem* cmditem_begin(struct cmditem *cmditem)
 *
 * - description
 * iterator function for get first item of cmditem
 *
 * - return value
 * lowest valued item will returned. if cmditem is empty, then the return
 * value is null
 */
struct cmditem* cmditem_begin(struct cmditem *cmditem);

/**
 * struct cmditem* cmditem_next(struct cmditem *cmditem)
 *
 * - description
 * iterator function that returns the next greater valued item
 *
 * - return value
 * it will returns the next greater item. if cmditem is the last item, null will
 * be returned
 */
struct cmditem* cmditem_next(struct cmditem *cmditem);

/**
 * void cmditem_free(struct cmditem*) - destruct cmditem
 */
void cmditem_free(struct cmditem *cmditem);

/* private functions */
static struct cmditem* least_child(struct cmditem *item);
static struct cmditem* greatest_child(struct cmditem *item);
static struct cmditem* nearest_greater_ancestor(struct cmditem *item);
static struct cmditem* grandparent(struct cmditem *item);

/* non-member functions */
/**
 * int cmditem_ctop(const char ch) - char to prio
 */
int cmditem_ctop(const char ch);
/**
 * int cmditem_stop(const char *str) - string to prio
 */
int cmditem_stop(const char *str);
/* for backward-compatibility */
int cmditem_to_prio(const char ch);
int cmditem_str_to_prio(const char *str);
/**
 * char cmditem_ptoc(int) - prio to char
 */
char cmditem_ptoc(int prio);

#endif /* _CMDITEM_H */
