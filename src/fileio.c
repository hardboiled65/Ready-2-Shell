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
    listfile->file = NULL;
}

int listfile_open(struct listfile *listfile, const char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0) {
        return LISTFILE_ERROR_NOT_FOUND;
    }

    listfile->file = fopen(filename, "a+");

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_readline(struct listfile *listfile, struct listitem *item)
{
    char buf;
    int file_offset = 0;

    if (listfile->file == NULL) {
        return LISTFILE_ERROR_NOT_OPENED;
    }
    
    /* read a line and write to the item */
    if ((buf = fgetc(listfile->file)) == EOF) {
        return LISTFILE_ERROR_FILE_EOF;
    }
    ++file_offset;
    /* read command */
    while (buf != ' ' && buf != '\t') {
        buf = fgetc(listfile->file);
        ++file_offset;
    }
    /* rewind to begin of the command name */
    fseek(listfile->file, -file_offset, SEEK_CUR);
    /* write command to item */
    item->cmd = (char*)malloc(sizeof(char*) * file_offset);
    fscanf(listfile->file, "%s", item->cmd);
    /* when after fscanf, seek goes last character of word that read */

    /* discard spaces or tabs */
    buf = fgetc(listfile->file);
    while (buf == ' ' || buf == '\t') {
        buf = fgetc(listfile->file);
    }

    /* read priority */
    /* buf is priority character (n, i, e) */
    if ((item->prio = listitem_to_prio(buf)) == -1) {
        return LISTFILE_ERROR_PARSE_ERROR;
    }

    /* discard spaces or tabs or a newline */
    buf = fgetc(listfile->file);
    while (buf == ' ' || buf == '\t') {
        buf = fgetc(listfile->file);
    }
    /* return immediately when no description */
    if (buf == '\n' || buf == EOF) {
        return LISTFILE_ERROR_NO_ERROR;
    }

    /* read description */
    file_offset = 0;
    ++file_offset;
    while (buf != '\n' && buf != EOF) {
        buf = fgetc(listfile->file);
        ++file_offset;
    }
    /* rewind to begin of the description */
    fseek(listfile->file, -file_offset, SEEK_CUR);
    /* write to item */
    item->desc = (char*)malloc(sizeof(char) * file_offset);
    fscanf(listfile->file, "%[^\n]", item->desc);

    /* move indicator to newline for next read */
    fgetc(listfile->file);

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_write(struct listfile *listfile)
{
    /* write to file */

    return LISTFILE_ERROR_NO_ERROR;
}

int listfile_close(struct listfile *listfile)
{
    if (listfile->file != NULL) {
        fclose(listfile->file);
        listfile->file = NULL;

        return LISTFILE_ERROR_NO_ERROR;
    } else {
        return LISTFILE_ERROR_NOT_OPENED;
    }
}

void listfile_free(struct listfile *listfile)
{
    if (listfile->file != NULL) {
        free(listfile->file);
        listfile->file = NULL;
    }
}

/* struct list */
void list_init(struct list *list)
{
    list->prev = NULL;
    list->next = NULL;
    list->item = (struct listitem*)malloc(sizeof(struct listitem*));
    listitem_init(list->item);
}

struct list* list_append(struct list *list, struct listitem *new_item)
{
    struct list *it;
    struct list *new_list;

    it = list;
    while (it->next != NULL) {
        it = it->next;
    }
    it->next = (struct list*)malloc(sizeof(struct list));
    new_list = it->next;

    /* init new list */
    new_list->prev = it;
    new_list->next = NULL;
    new_list->item = new_item;

    return new_list;
}

void list_free(struct list *list)
{
    /* destroy links */
    listitem_free(list->item);
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
