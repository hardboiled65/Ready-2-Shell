/*
//  whatis.c
//
//  Author:     Pale Purple
//  Created:    2016. 02. 18.
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#include "whatis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int whatis_is_installed()
{
    int res;

    res = (WEXITSTATUS(system(WHATIS_SHELL_COMMAND)) == 0) ? 1 : 0;

    return res;
}

/* unknown behavior when string length over 1024 in current implementation */
char* whatis_get(const char *cmd)
{
    char *system_str;
    int check_man;
    FILE *f;
    char *desc = NULL;
    int desc_len = 0;
    char ch;
    int i;
    char buf[1024];

    system_str = (char*)malloc(sizeof(char) *
                                (WHATIS_FORMAT_LEN + strlen(cmd) + 1));
    sprintf(system_str, WHATIS_FORMAT, cmd);
    check_man = WEXITSTATUS(system(system_str));
    free(system_str);
    /* return null immediately for no manpage */
    if (check_man != 0) {
        return desc;
    }

    system_str = (char*)malloc(sizeof(char) *
                                (WHATIS_SED_FORMAT_LEN + strlen(cmd) + 1));
    sprintf(system_str, WHATIS_SED_FORMAT, cmd);
    f = popen(system_str, "r");
    ch = fgetc(f);
    for (i = 0; (ch != '\n') && (ch != '\0'); ++i) {
        buf[i] = ch;
        desc_len += 1;
        ch = fgetc(f);
    }
    buf[i] = '\0';
    desc = (char*)malloc(sizeof(char) * (desc_len + 1));
    strcpy(desc, buf);

    return desc;
}
