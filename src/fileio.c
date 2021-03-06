/*
//  fileio.c
//
//  Author:     Pale Purple
//  Created:    2016. 01. 28. 03:37
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#include "fileio.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* struct listfile */
void listfile_init(struct listfile *listfile)
{
    listfile->r_file = NULL;
    listfile->w_file = NULL;
    listfile->lines = (struct lines*)malloc(sizeof(struct lines));
    lines_init(listfile->lines);
}

int listfile_open(struct listfile *listfile, const char *filename)
{
    char *default_path;
    struct stat st;

    default_path = (char*)malloc(sizeof(char) * (strlen(getenv("HOME"))
                                + strlen("/.r2shlist") + 1));
    sprintf(default_path, "%s/%s", getenv("HOME"), ".r2shlist");

    if ((stat(filename, &st) != 0) && (strcmp(default_path, filename) != 0)) {
        return LISTFILE_ERROR_NOT_FOUND;
    }
    free(default_path);

    listfile->r_file = fopen(filename, "r");

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_read(struct listfile *listfile)
{
    char buf;
    int file_offset = 0;
    char *temp;

    if (listfile->r_file == NULL) {
        return LISTFILE_ERROR_NOT_OPENED;
    }
    if (feof(listfile->r_file)) {
        return LISTFILE_ERROR_FILE_EOF;
    }

    while ((buf = fgetc(listfile->r_file)) != EOF) {
        ++file_offset;
        /* count characters of the line */
        while (buf != '\n' && buf != EOF) {
            buf = fgetc(listfile->r_file);
            ++file_offset;
        }
        /* rewind to begin of the line */
        /* if read is EOF, file seek not forwarded. so rewind 1 step less */
        if (buf != EOF) {
            fseek(listfile->r_file, -file_offset, SEEK_CUR);
        } else {
            fseek(listfile->r_file, -(file_offset - 1), SEEK_CUR);
        }
        /* read to memory */
        temp = (char*)malloc(sizeof(char) * file_offset);
        fgets(temp, file_offset, listfile->r_file);
        lines_append(listfile->lines, temp);

        fgetc(listfile->r_file);
        file_offset = 0;
    }

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_readline(struct listfile *listfile, struct listitem *item)
{
    char buf;
    int file_offset = 0;

    if (listfile->r_file == NULL) {
        return LISTFILE_ERROR_NOT_OPENED;
    }
    
    /* read a line and write to the item */
    if ((buf = fgetc(listfile->r_file)) == EOF) {
        return LISTFILE_ERROR_FILE_EOF;
    }
    ++file_offset;
    /* read command */
    while (buf != ' ' && buf != '\t') {
        buf = fgetc(listfile->r_file);
        ++file_offset;
    }
    /* rewind to begin of the command name */
    fseek(listfile->r_file, -file_offset, SEEK_CUR);
    /* write command to item */
    item->cmd = (char*)malloc(sizeof(char*) * file_offset);
    fscanf(listfile->r_file, "%s", item->cmd);
    /* when after fscanf, seek goes last character of word that read */

    /* discard spaces or tabs */
    buf = fgetc(listfile->r_file);
    while (buf == ' ' || buf == '\t') {
        buf = fgetc(listfile->r_file);
    }

    /* read priority */
    /* buf is priority character (n, i, e) */
    if ((item->prio = listitem_to_prio(buf)) == -1) {
        return LISTFILE_ERROR_PARSE_ERROR;
    }

    /* discard spaces or tabs or a newline */
    buf = fgetc(listfile->r_file);
    while (buf == ' ' || buf == '\t') {
        buf = fgetc(listfile->r_file);
    }
    /* return immediately when no description */
    if (buf == '\n' || buf == EOF) {
        return LISTFILE_ERROR_NO_ERROR;
    }

    /* read description */
    file_offset = 0;
    ++file_offset;
    while (buf != '\n' && buf != EOF) {
        buf = fgetc(listfile->r_file);
        ++file_offset;
    }
    /* rewind to begin of the description */
    fseek(listfile->r_file, -file_offset, SEEK_CUR);
    /* write to item */
    item->desc = (char*)malloc(sizeof(char) * file_offset);
    fscanf(listfile->r_file, "%[^\n]", item->desc);

    /* move indicator to newline for next read */
    fgetc(listfile->r_file);

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_write(struct listfile *listfile, const char *filename)
{
    int num;
    int i;

    listfile->w_file = fopen(filename, "w");

    if (listfile->w_file == NULL) {
        return LISTFILE_ERROR_NOT_OPENED;
    }

    num = listfile->lines->num;
    for (i = 0; i < num; ++i) {
        if (listfile->lines->data[i] != NULL) {
            fputs(listfile->lines->data[i], listfile->w_file);
            fputs("\n", listfile->w_file);
        }
    }

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_writeln(struct listfile *listfile, struct listitem *item)
{
    char *prio_char;

    /* write an item to file */
    fputs(item->cmd, listfile->r_file);
    fputs("\t", listfile->r_file);

    switch (item->prio) {
    case LISTITEM_IMPORTANT:
        prio_char = "i";
        break;
    case LISTITEM_NORMAL:
        prio_char = "n";
        break;
    case LISTITEM_EXTRA:
        prio_char = "e";
        break;
    default:
        break;
    }
    fputs(prio_char, listfile->r_file);
    fputs("\t", listfile->r_file);

    fputs(item->desc, listfile->r_file);
    fputs("\n", listfile->r_file);

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_close(struct listfile *listfile)
{
    if (listfile->r_file == NULL && listfile->w_file == NULL) {
        return LISTFILE_ERROR_NOT_OPENED;
    }

    if (listfile->r_file != NULL) {
        fclose(listfile->r_file);
        listfile->r_file = NULL;
    }
    if (listfile->w_file != NULL) {
        fclose(listfile->w_file);
        listfile->w_file = NULL;
    }
    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_size(struct listfile *listfile)
{
    return listfile->lines->num;
}

void listfile_free(struct listfile *listfile)
{
    if (listfile->r_file != NULL) {
        free(listfile->r_file);
        listfile->r_file = NULL;
    }
    if (listfile->w_file != NULL) {
        free(listfile->w_file);
        listfile->w_file = NULL;
    }
    if (listfile->lines != NULL) {
        lines_free(listfile->lines);
        listfile->lines = NULL;
    }
}

/* struct lines */
void lines_init(struct lines *lines)
{
    const int default_size = 50;

    lines->num = 0;
    lines->data = (char**)malloc(sizeof(char*) * default_size);
    lines->alloc_size = default_size;
}

int lines_add(struct lines *lines,
        const char *cmd, char prio, const char *desc)
{
    char *new_line;

    new_line = (char*)malloc(sizeof(char) * (
        strlen(cmd) + 1 /* cmd size + tab */
        + 1 + 1 /* prio size + tab */
        + ((desc != NULL) ? strlen(desc) : 0) /* desc size*/
        + 1)); /* null-character */
    sprintf(new_line, (desc != NULL ? "%s\t%c\t%s" : "%s\t%c%s"),
        cmd, prio, (desc != NULL ? desc : ""));

    /* append the new line */
    lines_append(lines, new_line);

    return lines->num;
}

void lines_append(struct lines *lines, char *line_str)
{
    char **new_data;
    int i;

    if (lines->num < lines->alloc_size) {
        lines->data[lines->num] = line_str;
    } else {
        new_data = (char**)malloc(sizeof(char*) * lines->alloc_size + 1);
        for (i = 0; i < lines->num; ++i) {
            new_data[i] = lines->data[i];
        }
        new_data[lines->alloc_size++] = line_str;
        free(lines->data);
        lines->data = new_data;
    }
    lines->num += 1;
}

void lines_erase(struct lines *lines, int idx)
{
    free(lines->data[idx]);
    lines->data[idx] = NULL;
}

void lines_free(struct lines *lines)
{
    int i;

    for (i = 0; i < lines->num; ++i) {
        if (lines->data[i] != NULL) {
            free(lines->data[i]);
        }
    }
    if (lines->data != NULL) {
        free(lines->data);
        lines->data = NULL;
    }
}

/* struct listitem */
void listitem_init(struct listitem *listitem)
{
    listitem->cmd = NULL;
    listitem->prio = 0;
    listitem->desc = NULL;
}

void listitem_set_cmd(struct listitem *listitem, const char *cmd, int size)
{
    listitem->cmd = (char*)malloc((sizeof(char) * size) + 1);
    strncpy(listitem->cmd, cmd, size);
}

void listitem_free(struct listitem *listitem)
{
    if (listitem->cmd != NULL) {
        free(listitem->cmd);
        listitem->cmd = NULL;
    }
    if (listitem->desc != NULL) {
        free(listitem->desc);
        listitem->desc = NULL;
    }
}

int listitem_to_prio(char ch)
{
    int prio;

    switch (ch) {
    case 'n':
    case 'N':
        prio = LISTITEM_NORMAL;
        break;
    case 'i':
    case 'I':
        prio = LISTITEM_IMPORTANT;
        break;
    case 'e':
    case 'E':
        prio = LISTITEM_EXTRA;
        break;
    default:
        prio = -1;
        break;
    }

    return prio;
}
