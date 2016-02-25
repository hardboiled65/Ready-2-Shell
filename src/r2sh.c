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
#include "cmditem.h"
#include "fileio.h"
#include "information.h"
#include "whatis.h"

static int flags;

int check_command(const char *cmd)
{
    char *system_command;
    int res;

    system_command = (char*)malloc(COMMAND_FORMAT_LEN + strlen(cmd) + 1);
    sprintf(system_command, COMMAND_FORMAT, cmd);

    res = WEXITSTATUS(system(system_command));

    return res;
}

void print_item(struct cmditem *item)
{
    char *circle_color;
    char *prio_str;

    if (item->prio == LISTITEM_IMPORTANT && is_set_important(flags)) {
        circle_color = COLOR_RED;
        prio_str = STRING_IMPORTANT;
    } else if (item->prio == LISTITEM_NORMAL && is_set_normal(flags)) {
        circle_color = COLOR_YELLOW;
        prio_str = STRING_NORMAL;
    } else if (item->prio == LISTITEM_EXTRA && is_set_extra(flags)) {
        circle_color = COLOR_BLUE;
        prio_str = STRING_EXTRA;
    } else {
        return;
    }
    if (item->cmd == NULL) {
        return;
    }
    if (check_command(item->cmd) == 0) {
        if (is_set_verbose(flags)) {
            printf("%-12s  %s●%s %s%-9s%s --> %s[OK]           %s  %s\n",
                item->cmd,
                circle_color, COLOR_RESET,
                COLOR_BOLD, prio_str, COLOR_RESET,
                COLOR_GREEN, COLOR_RESET,
                (item->desc != NULL) ? item->desc : "");
        }
    } else {
        printf("%-12s  %s●%s %s%-9s%s --> %s[NOT INSTALLED]%s  %s\n",
            item->cmd,
            circle_color, COLOR_RESET,
            COLOR_BOLD, prio_str, COLOR_RESET,
            COLOR_RED, COLOR_RESET,
            (item->desc != NULL) ? item->desc : "");
    }
}

void add_mode(struct listfile *listfile, struct args *args)
{
    char cmd_input[1024];
    char prio_input[3];
    char desc_input[2048];
    char *desc_auto;
    struct listitem item;

    /* check which argument is not passed and ask to input */
    if (args->text.cmd == NULL) {
        printf("command: ");
        console_input_s(cmd_input, 1024);
        while (strlen(cmd_input) == 0) {
            printf("command: ");
            console_input_s(cmd_input, 1024);
        }
        if (check_command(cmd_input) != 0) {
            printf("%s%s%s is not installed. if this is a misspelling, Ctrl + C to cancel.\n",
                COLOR_BOLD, cmd_input, COLOR_RESET);
        }
        args->text.cmd = cmd_input;
    }
    if (args->text.prio == NULL) {
        printf("priority [0: important / 1: normal / 2: extra]: (1) ");
        console_input_s(prio_input, 2);
        /* default is normal(1) */
        if (strlen(prio_input) == 0) {
            strcpy(prio_input, "1");
        }
        args->text.prio = prio_input;
    }
    if (args->text.desc == NULL) {
        /* get short description with whatis */
        desc_auto = whatis_get(args->text.cmd);
        if (desc_auto != NULL) {
            printf("description: (%s) ", desc_auto);
        } else {
            printf("description: () ");
        }
        console_input_s(desc_input, 2048);
        if (strlen(desc_input) == 0) {
            /* use short description that automatically obtained or "" if not */
            if (desc_auto != NULL) {
                args->text.desc = desc_auto;
            } else {
                strcpy(desc_input, "");
                args->text.desc = desc_input;
            }
        } else {
            args->text.desc = desc_input;
        }
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

void modify_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem)
{
    char prio_input[3];
    char desc_input[2048];
    struct cmditem *found;
    int new_str_len;
    char *new_str;
    char prio_char;

    /* print error message if command not specified */
    if (args->text.cmd == NULL) {
        printf("error: command not selected\n");
        return;
    }
    /* show and input new information */
    if ((found = cmditem_find(cmditem, args->text.cmd)) == NULL) {
        printf("error: %s: no such command in the list\n", args->text.cmd);
        return;
    }
    printf("command: (%s)\n", found->cmd);
    if (args->text.prio == NULL) {
        printf("priority [0: important / 1: normal / 2: extra]: (%d) ",
                found->prio);
        console_input_s(prio_input, 2);
        if (strcmp(prio_input, "") != 0) {
            found->prio = atoi(prio_input);
        }
    } else {
        found->prio = atoi(args->text.prio);
    }
    if (args->text.desc == NULL) {
        printf("description: (%s) ", found->desc);
        console_input_s(desc_input, 2048);
        if (strcmp(desc_input, "") != 0) {
            // args->text.desc = desc_input;
            if (cmditem->desc != NULL) {
                free(cmditem->desc);
                found->desc = NULL;
            }
            found->desc = (char*)malloc( (sizeof(char) * strlen(desc_input))
                                            + 1 );
            strcpy(found->desc, desc_input);
        }
    } else {
        if (found->desc != NULL) {
            free(found->desc);
            found->desc = NULL;
        }
        found->desc = (char*)malloc( (sizeof(char) * strlen(args->text.desc))
                                        + 1 );
        strcpy(found->desc, args->text.desc);
    }
    printf("[%s] [%d] [%s]\n", found->cmd, found->prio, found->desc);
    new_str_len = strlen(found->cmd);
    new_str_len += 2; /* tab and prio */
    new_str_len += (found->desc != NULL) ? 1 + strlen(found->desc) : 0;
                      /* tab and desc if description exists */
    new_str_len += 1; /* null-char */
    new_str = (char*)malloc(sizeof(char) * new_str_len);
    switch (found->prio) {
    case CMDITEM_IMPORTANT:
        prio_char = 'i';
        break;
    case CMDITEM_NORMAL:
        prio_char = 'n';
        break;
    case CMDITEM_EXTRA:
        prio_char = 'e';
        break;
    default:
        break;
    }
    if (found->desc != NULL) {
        sprintf(new_str, "%s\t%c\t%s", found->cmd, prio_char, found->desc);
    } else {
        sprintf(new_str, "%s\t%c", found->cmd, prio_char);
    }
    free(listfile->lines->data[found->line - 1]);
    listfile->lines->data[found->line - 1] = new_str;
}

void delete_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem)
{
    char cmd_input[1024];
    struct cmditem *found;

    if (args->text.cmd == NULL) {
        printf("command: ");
        console_input_s(cmd_input, 1024);
        /* if input nothing, show input prompt again */
        while (strlen(cmd_input) == 0) {
            printf("command: ");
            console_input_s(cmd_input, 1024);
        }
    }
    if (args->text.cmd == NULL) {
        found = cmditem_find(cmditem, cmd_input);
    } else {
        found = cmditem_find(cmditem, args->text.cmd);
    }
    if (found == NULL) {
        printf("error: no such command\n");
        return;
    }
    free(listfile->lines->data[found->line - 1]);
    listfile->lines->data[found->line - 1] = NULL;
    printf("deleted: [%s] [%d] [%s]\n", found->cmd, found->prio, found->desc);
    /* TODO: also delete from cmditem */
}

void output_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem)
{
    struct cmditem *it;
    FILE *output;

    if (args->text.out != NULL) {
        output = fopen(args->text.out, "w");
    } else {
        output = stdout;
    }

    it = cmditem_begin(cmditem);
    while (it != NULL) {
        if ((it->prio == CMDITEM_IMPORTANT && is_set_important(args->flags)) ||
            (it->prio == CMDITEM_NORMAL && is_set_normal(args->flags)) ||
            (it->prio == CMDITEM_EXTRA && is_set_extra(args->flags))) {
            if (check_command(it->cmd) != 0) {
                fprintf(output, "%s ", it->cmd);
            }
        }
        it = cmditem_next(it);
    }
    fprintf(output, "\n");
}

/*
void console_input(char *dst, int size)
{
    size_t str_len;
    int read_len = 0;

    fgets(dst, size, stdin);
    str_len = strlen(dst);

    if (dst[str_len - 1] == '\n') {
        dst[str_len - 1] = '\0';
    }
    read_len = str_len;
}
*/

void console_input_s(char *dst, int size)
{
    char buf;
    int i;

    for (i = 0; i < size; ++i) {
        buf = fgetc(stdin);
        if (buf != '\n') {
            dst[i] = buf;
        } else {
            dst[i] = '\0';
            break;
        }
    }
    /* if not encountered to newline, discard characters in buffer */
    if (i == size) {
        dst[size - 1] = '\0';
        while (fgetc(stdin) != '\n') {}
    }
}

int main(int argc, char *argv[])
{
    struct args *args;
    struct listfile listfile;
    char *listfile_path;
    int err;
    struct listitem *item;
    char *error_str;        /* for perror */
    struct cmditem cmditem;
    int i;
    struct cmditem *cmditem_tmp;

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
        printf("%s\n", INFORMATION_VERSION);
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
        if (args->text.file == NULL) {
            printf("%s: missing filename\n", argv[0]);
            return 1;
        }
        listfile_path = (char*)malloc(sizeof(char) * (strlen(args->text.file)
                                    + 1));
        sprintf(listfile_path, "%s", args->text.file);
    }
    if (listfile_open(&listfile, listfile_path) != LISTFILE_ERROR_NO_ERROR) {
        error_str = (char*)malloc(sizeof(char) * (strlen(argv[0]) + strlen(": ")
                                    + strlen(args->text.file) + 1));
        sprintf(error_str, "%s: %s", argv[0], args->text.file);
        perror(error_str);
        return 1;
    }
    /* read file and parse that */
    listfile_read(&listfile);
    cmditem_init(&cmditem);
    for (i = 0; i < listfile.lines->num; ++i) {
        if ((listfile.lines->data[i] != NULL)
                && (strlen(listfile.lines->data[i]) != 0)) {
            if (i == 0) { /* for root */
                cmditem_tmp = &cmditem;
            } else {
                cmditem_tmp = (struct cmditem*)malloc(sizeof(struct cmditem));
                cmditem_init(cmditem_tmp);
            }
            err = cmditem_parse_string(cmditem_tmp, listfile.lines->data[i]);
            if (err == CMDITEM_ERROR_PARSE_ERROR) {
                printf("parsing error: line %d\n", i + 1);
                printf("%s\n", listfile.lines->data[i]);
                return 1;
            }
            cmditem_tmp->line = i + 1;
            if (i != 0) {
                cmditem_append(&cmditem, cmditem_tmp);
            }
        }
    }

    /* enter modifying mode if mode is add/edit */
    if (args->mode == ARGS_EDIT) {
        if (args->flags & FLAGS_ADD) {
            add_mode(&listfile, args);
        } else if (args->flags & FLAGS_MODIFY) {
            modify_mode(&listfile, args, &cmditem);
            listfile_write(&listfile, listfile_path);
        } else if (args->flags & FLAGS_DELETE) {
            delete_mode(&listfile, args, &cmditem);
            listfile_write(&listfile, listfile_path);
        }
        return 0;
    }

    /* enter output mode if mode is output */
    if (args->mode == ARGS_OUTPUT) {
        output_mode(&listfile, args, &cmditem);
        return 0;
    }

    /* if mode is print, scan r2shlist file and print results. */

    flags = args->flags;
    cmditem_traversal(&cmditem, print_item);

    cmditem_free(&cmditem);
    listfile_close(&listfile);
    listfile_free(&listfile);

    return 0;
}
