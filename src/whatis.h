/*
//  whatis.h
//
//  Author:     Pale Purple
//  Created:    2016. 02. 18.
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//  Get short description by using `whatis` shell command for default desc when
//  adding new command.
*/
#ifndef _WHATIS_H
#define _WHATIS_H

#define WHATIS_SHELL_COMMAND    "command -v whatis >> /dev/null"
#define WHATIS_FORMAT           "whatis %s >/dev/null 2>/dev/null"
/*                         30 -> |______  ______________________| */
#define WHATIS_FORMAT_LEN       30
#define WHATIS_SED_FORMAT       "whatis %s |head -1 |sed -r -e 's/.* - //'"
/*                         39 -> |______  _______________________________| */
#define WHATIS_SED_FORMAT_LEN   39

/**
 * int whatis_is_installed() - check whatis command installed or not installed
 *
 * - return value
 * 0 for not installed, 1 or non-zero value for installed
 */
int whatis_is_installed();
/**
 * char* whatis_get(const char*)
 *
 * - description
 * get command string as a parameter, then return the pointer to the description
 * string. the string is dynamic allocated char pointer so you should free
 * manually when end of life.
 * - return value
 * pointer to null-terminated string for success. null ptr returned if failed.
 */
char* whatis_get(const char *cmd);

#endif /* _WHATIS_H */
