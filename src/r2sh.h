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

struct listitem;

int check_command(const char *cmd);
void print(struct listitem *item, int flags);

#endif /* _R2SH_H */
