/*
//  r2sh.h
//
//  Author:     Pale Purple
//  Created:    2016. 01. 28. 17:12
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#ifndef _R2SH_H
#define _R2SH_H

#define COMMAND_FORMAT      "command -v %s >> /dev/null"
/*                     24 -> |__________  ____________| */
#define COMMAND_FORMAT_LEN  24

#define STRING_IMPORTANT    "IMPORTANT"
#define STRING_NORMAL       "NORMAL"
#define STRING_EXTRA        "EXTRA"

#define COLOR_RESET         "\e[00m"
#define COLOR_RED           "\e[31m"
#define COLOR_GREEN         "\e[32m"
#define COLOR_YELLOW        "\e[1;33m"
#define COLOR_BLUE          "\e[34m"
#define COLOR_BOLD          "\e[1m"

struct args;
struct cmditem;
struct listfile;
struct listitem;

int check_command(const char *cmd);
void print_item(struct cmditem *item);
void add_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem);
void modify_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem);
void delete_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem);
void output_mode(struct listfile *listfile, struct args *args,
    struct cmditem *cmditem);

/**
 * console_input_s(char* dst, int size)
 *
 * read a line from console input, and leftovers are stripped
 * - parameters
 * dst      destination char pointer that input string stored
 * size     maximum read size
 */
void console_input_s(char *dst, int size);

#endif /* _R2SH_H */
