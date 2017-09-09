/*
//  args.h
//
//  Author:     Pale Purple
//  Created:    2016. 01. 28. 17:16
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#ifndef _ARGS_H
#define _ARGS_H

#define ARGS_HELP_SHORT         'h'
#define ARGS_HELP_LONG          "help"
#define ARGS_VERSION_SHORT      'V'
#define ARGS_VERSION_LONG       "version"

#define ARGS_FILE_PATH_SHORT    'f'
#define ARGS_FILE_PATH_LONG     "file"
#define ARGS_OUTPUT_SHORT       'o'
#define ARGS_OUTPUT_LONG        "output"

#define ARGS_PRINT_ALL_SHORT    'a'
#define ARGS_PRINT_ALL_LONG     "all"
#define ARGS_PRINT_IMPORTANT_SHORT  'i'
#define ARGS_PRINT_IMPORTANT_LONG   "important"
#define ARGS_PRINT_NORMAL_SHORT 'n'
#define ARGS_PRINT_NORMAL_LONG  "normal"
#define ARGS_PRINT_EXTRA_SHORT  'x'
#define ARGS_PRINT_EXTRA_LONG   "extra"
#define ARGS_PRINT_VERBOSE_SHORT    'v'
#define ARGS_PRINT_VERBOSE_LONG "verbose"

#define ARGS_EDIT_ADD_SHORT     'A'
#define ARGS_EDIT_MODIFY_SHORT  'M'
#define ARGS_EDIT_DELETE_SHORT  'D'
#define ARGS_EDIT_COMMAND       'c'
#define ARGS_EDIT_PRIORITY      'p'
#define ARGS_EDIT_DESCRIPTION   'd'
/*
#define ARGS_EDIT_MODE_SHORT    'e'
#define ARGS_EDIT_FILE_SHORT    'E'
#define ARGS_EDIT_FILE_LONG     "edit-file"
*/

/**
 * macro ARGS_COMPARE_LONG(arg, longstr)
 *
 * true if arg same as longstr
 */
#define args_compare_long(arg, longstr) \
    (strncmp((arg) + 2, (longstr), strlen(longstr)) == 0)

/**
 * option flags
 *
 * 1-2 - information option
 *       1 - help           print help message
 *       2 - version        print version
 * 3-6 - print option
 *       3 - important      check then print important commands
 *       4 - normal         check then print normal commands
 *       5 - extra          check then print extra commands
 *       6 - verbose        also print flaged exist commands
 * 7-8 - specified input/output file
 *       7 - input          set input file path explicitely
 *       8 - output         set output file path explicitely
 * 9-11 - add/modify/delete - cannot multiple set
 *       9 - add            add new command
 *      10 - modify         modify exist command's prio/descr
 *      11 - delete         delete a command
 */
#define FLAGS_HELP      0x01
#define FLAGS_VERSION   0x02
#define FLAGS_IMPORTANT 0x04
#define FLAGS_NORMAL    0x08
#define FLAGS_EXTRA     0x10
#define FLAGS_VERBOSE   0x20
#define FLAGS_INPUT     0x40
#define FLAGS_OUTPUT    0x80
#define FLAGS_ADD       0x100
#define FLAGS_MODIFY    0x200
#define FLAGS_DELETE    0x400

/**
 * macro is_xx_mode, is_set_xx
 */
#define is_print_mode(flags) \
    (!((flags) & FLAGS_HELP) && !((flags) & FLAGS_VERSION) \
    && !((flags) & FLAGS_ADD) && !((flags) & FLAGS_MODIFY) \
    && !((flags) & FLAGS_DELETE) && !((flags) & FLAGS_OUTPUT))

#define is_help_mode(flags) \
    ((flags) & FLAGS_HELP)

#define is_version_mode(flags) \
    (((flags) & FLAGS_VERSION) && !((flags) & FLAGS_HELP))

#define is_edit_mode(flags) \
    (((flags) & FLAGS_ADD) \
    || ((flags) & FLAGS_MODIFY) \
    || ((flags) & FLAGS_DELETE))

#define is_output_mode(flags) \
    (((flags) & FLAGS_OUTPUT))

#define is_set_important(flags) \
    ((flags) & FLAGS_IMPORTANT)

#define is_set_normal(flags) \
    ((flags) & FLAGS_NORMAL)

#define is_set_extra(flags) \
    ((flags) & FLAGS_EXTRA)

#define is_set_verbose(flags) \
    ((flags) & FLAGS_VERBOSE)

#define is_set_input(flags) \
    ((flags) & FLAGS_INPUT)

#define is_set_output(flags) \
    ((flags) & FLAGS_OUTPUT)

struct args;
struct args_text;

/**
 * struct args_text - text arguments
 *
 * - members
 * file     input file
 * out      output file
 * cmd      command name
 * prio     priority
 * desc     description
 */
struct args_text {
    const char *file;
    const char *out;
    const char *cmd;
    const char *prio;
    const char *desc;
};

void args_text_init(struct args_text *args_text);

/**
 * struct args - arguments class
 *
 * - members
 * count    options count
 * flags    bit flags of option
 * wrong    position of wrong(invalid) option in argv
 *          -1 for nothing wrong
 * text     text arguments such as filename
 * mode     selected operation mode
 */
struct args {
    int count;
    int flags;
    int wrong;
    struct args_text text;
    int mode;
};

enum args_mode {
    ARGS_PRINT = 0,
    ARGS_HELP = 1,
    ARGS_VERSION = 2,
    ARGS_EDIT = 3,
    ARGS_OUTPUT = 4,
};

void args_init(struct args *args);
void args_set_mode(struct args *args);
int args_is_wrong(struct args *args);
int args_is_set_cmd(struct args *args);

/**
 * struct args* parse_args(int, char**)
 *
 * get parameters from main() and process it
 * - return value
 * A pointer to heap allocated 'struct args' that contains informations about
 * which operation should be executed.
 */
struct args* parse_args(int argc, char *argv[]);

/**
 * const char* strip_long_arg(const char *arg, const char *option)
 *
 * strip '--option=' from given long option
 * - parameters
 * arg      element of argv
 * option   pre-defined option string
 * - return value
 * if available, the function returns the starting position of user argument
 * value. if unavailable, it returns null pointer
 */
const char* strip_long_arg(const char *arg, const char *option);

#endif /* _ARGS_H */
