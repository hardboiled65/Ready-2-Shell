/*
//  args.c
//
//  Author:     Pale Purple
//  Created:    2016. 01. 30.
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#include "args.h"

#include <stdlib.h>
#include <string.h>

void args_text_init(struct args_text *args_text)
{
    args_text->file = NULL;
    args_text->out = NULL;
    args_text->cmd = NULL;
    args_text->prio = NULL;
    args_text->desc = NULL;
}

void args_init(struct args *args)
{
    args->count = 0;
    args->flags = 0;
    args->wrong = -1;
    args_text_init(&(args->text));
    args->mode = 0;
}

void args_set_mode(struct args *args)
{
    if (is_print_mode(args->flags)) {
        args->mode = ARGS_PRINT;
    } else if (is_help_mode(args->flags)) {
        args->mode = ARGS_HELP;
    } else if (is_version_mode(args->flags)) {
        args->mode = ARGS_VERSION;
    } else if (is_edit_mode(args->flags)) {
        args->mode = ARGS_EDIT;
    }
}

struct args* parse_args(int argc, char *argv[])
{
    struct args *args = (struct args*)malloc(sizeof(struct args));
    int i;

    /* TODO: error handling when '-f' option set but file not specified */
    /* init struct args */
    args_init(args);
    args->count = argc - 1;

    /* set flags */
    for (i = 1; i < argc; ++i) {
        /* argument is an option */
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            /* short options */
            case ARGS_HELP_SHORT:
                args->flags |= FLAGS_HELP;
                break;
            case ARGS_VERSION_SHORT:
                args->flags |= FLAGS_VERSION;
                break;
            case ARGS_FILE_PATH_SHORT:
                args->flags |= FLAGS_INPUT;
                if ((i + 1) < argc) args->text.file = argv[i + 1];
                break;
            case ARGS_OUTPUT_SHORT:
                args->flags |= FLAGS_OUTPUT;
                if ((i + 1) < argc) args->text.out = argv[i + 1];
                break;
            case ARGS_PRINT_ALL_SHORT:
                args->flags |= FLAGS_IMPORTANT;
                args->flags |= FLAGS_NORMAL;
                args->flags |= FLAGS_EXTRA;
                break;
            case ARGS_PRINT_IMPORTANT_SHORT:
                args->flags |= FLAGS_IMPORTANT;
                break;
            case ARGS_PRINT_NORMAL_SHORT:
                args->flags |= FLAGS_NORMAL;
                break;
            case ARGS_PRINT_EXTRA_SHORT:
                args->flags |= FLAGS_EXTRA;
                break;
            case ARGS_PRINT_VERBOSE_SHORT:
                args->flags |= FLAGS_VERBOSE;
                break;
            case ARGS_EDIT_ADD_SHORT:
                args->flags |= FLAGS_ADD;
                break;
            case ARGS_EDIT_MODIFY_SHORT:
                args->flags |= FLAGS_MODIFY;
                break;
            case ARGS_EDIT_DELETE_SHORT:
                args->flags |= FLAGS_DELETE;
                break;
            case ARGS_EDIT_COMMAND:
                if ( ((i + 1) < argc) && (argv[i + 1][0] != '-') ) {
                    args->text.cmd = argv[i + 1];
                }
                break;
            case ARGS_EDIT_PRIORITY:
                if ( ((i + 1) < argc) && (argv[i + 1][0] != '-') ) {
                    args->text.prio = argv[i + 1];
                }
                break;
            case ARGS_EDIT_DESCRIPTION:
                if ( ((i + 1) < argc) && (argv[i + 1][0] != '-') ) {
                    args->text.desc = argv[i + 1];
                }
                break;
            /* long options */
            case '-':
                if (args_compare_long(argv[i], ARGS_HELP_LONG)) {
                    args->flags |= FLAGS_HELP;
                } else if (args_compare_long(argv[i], ARGS_VERSION_LONG)) {
                    args->flags |= FLAGS_VERSION;
                } else if (args_compare_long(argv[i], ARGS_FILE_PATH_LONG)) {
                    args->flags |= FLAGS_INPUT;
                    args->text.file = strip_long_arg(argv[i], ARGS_FILE_PATH_LONG);
                } else if (args_compare_long(argv[i], ARGS_OUTPUT_LONG)) {
                    args->flags |= FLAGS_OUTPUT;
                    args->text.out = strip_long_arg(argv[i], ARGS_OUTPUT_LONG);
                } else if (args_compare_long(argv[i], ARGS_PRINT_ALL_LONG)) {
                    args->flags |= FLAGS_IMPORTANT;
                    args->flags |= FLAGS_NORMAL;
                    args->flags |= FLAGS_EXTRA;
                } else if (args_compare_long(argv[i], ARGS_PRINT_IMPORTANT_LONG)) {
                    args->flags |= FLAGS_IMPORTANT;
                } else if (args_compare_long(argv[i], ARGS_PRINT_NORMAL_LONG)) {
                    args->flags |= FLAGS_NORMAL;
                } else if (args_compare_long(argv[i], ARGS_PRINT_EXTRA_LONG)) {
                    args->flags |= FLAGS_EXTRA;
                } else if (args_compare_long(argv[i], ARGS_PRINT_VERBOSE_LONG)) {
                    args->flags |= FLAGS_VERBOSE;
                } else {
                    /* wrong option position set if not match any options */
                    args->wrong = i;
                }
                break;
            default:
                /* wrong option position set if not match any short options */
                args->wrong = i;
                break;
            } /* switch */
        /* argument is a suboption */
        } else {
        } /* end if (argv[i][0] == '-') */
    } /* end for */
    /* default is important/normal set */
    if (!(is_set_important(args->flags)) && !(is_set_normal(args->flags))
            && !(is_set_extra(args->flags))) {
        args->flags |= FLAGS_IMPORTANT;
        args->flags |= FLAGS_NORMAL;
    }

    /* set mode */
    args_set_mode(args);

    return args;
}

const char* strip_long_arg(const char *arg, const char *option)
{
    /**
     * '+ 3' for "--" and "="
     * if arg only "--option="(equal to '+3') or "--option"(less than '+3'),
     * it will returns null ptr
     */
    if (strlen(arg) > strlen(option) + 3) {
        return arg + (strlen(option) + 3);
    } else {
        return NULL;
    }
}
