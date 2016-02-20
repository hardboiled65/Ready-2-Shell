/*
//  fileio.h
//
//  Author:     Pale Purple
//  Created:    2016. 01. 28. 02:56
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#ifndef _FILEIO_H
#define _FILEIO_H

#include <stdio.h>

struct listfile;
struct lines;
struct listitem;

/**
 * struct listfile - a file saved selected commands
 *
 * - description
 * class for a file to open, write, close.
 */
struct listfile {
    FILE *r_file;
    FILE *w_file;
    struct lines *lines;
};

enum listfile_error {
    LISTFILE_ERROR_NO_ERROR = 0,
    LISTFILE_ERROR_NOT_FOUND = 1,
    LISTFILE_ERROR_NOT_OPENED = 2,
    LISTFILE_ERROR_FILE_EOF = 3,
    LISTFILE_ERROR_PARSE_ERROR = 4,
};

void listfile_init(struct listfile *listfile);
int listfile_open(struct listfile *listfile, const char *filename);
int listfile_read(struct listfile *listfile);
int listfile_readline(struct listfile *listfile, struct listitem *item);
int listfile_write(struct listfile *listfile, const char *filename);
int listfile_writeln(struct listfile *listfile, struct listitem *item);
int listfile_close(struct listfile *listfile);
void listfile_free(struct listfile *listfile);

/**
 * struct lines - store data of file line by line
 *
 * - members
 * num          number of lines
 * alloc_size   allocated size of array
 * data         array to line data
 */
struct lines {
    int num;
    int alloc_size;
    char **data;
};

void lines_init(struct lines *lines);
void lines_append(struct lines *lines, char *line_str);
void lines_free(struct lines *lines);

/**
 * struct listitem - an item about the command
 *
 * - members
 * cmd      name of command
 * prio     priority (normal / important / extra).
 * desc     user description
 */
struct listitem {
    char *cmd;
    int prio;
    char *desc;
};

enum listitem_priority {
    LISTITEM_IMPORTANT = 0,
    LISTITEM_NORMAL = 1,
    LISTITEM_EXTRA = 2
};

void listitem_init(struct listitem *listitem);
void listitem_set_cmd(struct listitem *listitem, const char *cmd, int size);
void listitem_free(struct listitem *listitem);

/* return listitem_priority enum or -1 if error */
int listitem_to_prio(char ch);

#endif /* _FILEIO_H */
