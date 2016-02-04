/*
//  r2sh.c
//
//  Author:     Pale Purple
//  Created:    2016. 01. 28. 17:13
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#include "r2sh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "fileio.h"

int check_command(const char *cmd)
{
    char *system_command;
    int res;

    system_command = (char*)malloc(COMMAND_FORMAT_LEN + strlen(cmd) + 1);
    sprintf(system_command, COMMAND_FORMAT, cmd);

    res = WEXITSTATUS(system(system_command));

    return res;
}

void print(struct listitem *item, int flags)
{
    if (item->prio == LISTITEM_IMPORTANT && is_set_important(flags)) {
    } else if (item->prio == LISTITEM_NORMAL && is_set_normal(flags)) {
    } else if (item->prio == LISTITEM_EXTRA && is_set_extra(flags)) {
    } else {
        return;
    }
    if (check_command(item->cmd) == 0) {
        if (is_set_verbose(flags)) {
            printf("[%s]\t[%d]\t[%s]", item->cmd, item->prio, item->desc);
            printf("\t--> [OK]\n");
        }
    } else {
        printf("[%s]\t[%d]\t[%s]", item->cmd, item->prio, item->desc);
        printf("\t--> [NOT INSTALLED]\n");
    }
}

int main(int argc, char *argv[])
{
    struct args *args;
    struct listfile listfile;
    char *listfile_path;
    struct list *list;
    int read_result;
    struct listitem *item;
    struct list *list_it;   /* for print */
    char *error_str;        /* for perror */

    args = parse_args(argc, argv);

    /* print help and exit if mode is help */
    if (args->mode == ARGS_HELP) {
        printf("help\n");
        return 0;
    }
    /* print version and exit if mode is version */
    if (args->mode == ARGS_VERSION) {
        printf("version\n");
        return 0;
    }

    /* if mode is print, scan r2shlist file and print results. */
    listfile_init(&listfile);

    if ( !(is_set_input(args->flags)) ) {
        listfile_path = (char*)malloc(sizeof(char) * (strlen(getenv("HOME"))
                                    + strlen("/.r2shlist") + 1));
        sprintf(listfile_path, "%s/%s", getenv("HOME"), ".r2shlist");
    } else {
        /* input flag set but filename missing */
        if (args->text.in == NULL) {
            printf("%s: missing filename\n", argv[0]);
            return 1;
        }
        listfile_path = (char*)malloc(sizeof(char) * (strlen(args->text.in)
                                    + 1));
        sprintf(listfile_path, "%s", args->text.in);
    }
    if (listfile_open(&listfile, listfile_path) != LISTFILE_ERROR_NO_ERROR) {
        error_str = (char*)malloc(sizeof(char) * (strlen(argv[0]) + strlen(": ")
                                    + strlen(args->text.in) + 1));
        sprintf(error_str, "%s: %s", argv[0], args->text.in);
        perror(error_str);
        return 1;
    }

    /* create list */
    list = (struct list*)malloc(sizeof(struct list));
    list_init(list);

    item = (struct listitem*)malloc(sizeof(struct listitem));
    listitem_init(item);
    read_result = listfile_readline(&listfile, item);
    while (read_result != LISTFILE_ERROR_FILE_EOF) {
        list_append(list, item);
        item = (struct listitem*)malloc(sizeof(struct listitem));
        listitem_init(item);
        read_result = listfile_readline(&listfile, item);
    }

    list_it = list->next;
    while (list_it != NULL) {
        print(list_it->item, args->flags);
        list_it = list_it->next;
    }

    listfile_close(&listfile);
    listfile_free(&listfile);

    return 0;
}
