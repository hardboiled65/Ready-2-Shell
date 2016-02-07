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
#include "information.h"

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

void add_mode(struct listfile *listfile, struct args *args)
{
    char cmd_input[1024];
    char prio_input[3];
    char desc_input[2048];
    struct listitem item;

    /* check which argument is not passed and ask to input */
    if (args->text.cmd == NULL) {
        printf("command: ");
        fgets(cmd_input, 1024, stdin);
        /* TODO: if input nothing, show input prompt again */
        if (cmd_input[strlen(cmd_input) - 1] == '\n') {
            cmd_input[strlen(cmd_input) - 1] = '\0';
        }
        args->text.cmd = cmd_input;
    }
    if (args->text.prio == NULL) {
        printf("priority [0: important / 1: normal / 2: extra]: (1) ");
        fgets(prio_input, 3, stdin);
        /* default is normal(1) */
        if (prio_input[0] == '\n') {
            strcpy(prio_input, "1");
        }
        if (prio_input[1] == '\n') {
            prio_input[1] = '\0';
        }
        args->text.prio = prio_input;
    }
    if (args->text.desc == NULL) {
        printf("description: () ");
        fgets(desc_input, 2048, stdin);
        /* default is "" */
        if (desc_input[0] == '\n') {
            strcpy(desc_input, "");
        }
        if (desc_input[strlen(desc_input) - 1] == '\n') {
            desc_input[strlen(desc_input) - 1] = '\0';
        }
        args->text.desc = desc_input;
    }
    printf("[%s] [%s] [%s]\n", args->text.cmd, args->text.prio, args->text.desc);

    listitem_init(&item);
    item.cmd = args->text.cmd;
    item.desc = args->text.desc;
    if (args->text.prio[0] == '0') {
        item.prio = LISTITEM_IMPORTANT;
    } else if (args->text.prio[0] == '1') {
        item.prio = LISTITEM_NORMAL;
    } else if (args->text.prio[0] == '2') {
        item.prio = LISTITEM_EXTRA;
    }
    listfile_writeln(listfile, &item);
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

    /* print error and return error code when wrong position set */
    if (args->wrong != -1) {
        printf("%s: invalid option '%s'\n", argv[0], argv[args->wrong]);
        return 1;
    }
    /* print help and exit if mode is help */
    if (args->mode == ARGS_HELP) {
        printf("%s", INFORMATION_HELP);
        return 0;
    }
    /* print version and exit if mode is version */
    if (args->mode == ARGS_VERSION) {
        printf("version\n");
        return 0;
    }

    /* init listfile and open the file if mode needs the file */
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

    /* enter add mode if mode is add/edit TODO: fix*/
    if (args->mode == ARGS_EDIT) {
        add_mode(&listfile, args);
        return 0;
    }

    /* if mode is print, scan r2shlist file and print results. */

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
