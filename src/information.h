/*
//  information.h
//
//  Author:     Pale Purple
//  Created:    2016. 01. 30.
//  Copyright (c) 2016 Pale Purple. All rights reserved.
//
//
*/
#ifndef _INFORMATION_H
#define _INFORMATION_H

static const char* INFORMATION_HELP =
    "Usage: r2sh [option] [-f file] [-o output]\n"
    // "       r2sh [-e]\n"
    "scan .r2shlist file and print not installed commands.\n"
    "default is same as 'r2sh -i -n'\n"
/*
  -B, --base=BASE               INDENTATION BASE LINE
    "  -B, --base=BASE               INDENTATION BASE LINE"
*/
    "\n"
    "print options:\n"
    "  -a, --all                     print all include extra priority commands\n"
    "  -i, --important               important priority commands\n"
    "  -n, --normal                  normal priority commands\n"
    "  -v, --verbose                 also print already installed commands\n"
    "  -x, --extra                   extra priority command\n"
    "other options:\n"
    "  -A                            add new command to list\n"
    "  -c                            with -A or -M or -D, name of command\n"
    "  -d                            with -A or -M, description of command\n"
    "  -D                            delete a command from list\n"
    // "  -e                            enter editor mode"
    // "  -E, --edit-file=FILE          editor mode with alternative file"
    "  -f, --file=FILE               use alternative input file\n"
    "  -h, --help                    print this help message and exit\n"
    "  -M                            modify a command\n"
    "  -o, --output=OUTPUT           write to file only command names that separated\n"
    "                                by space\n"
    "                                without OUTPUT file, it will print to standard\n"
    "                                output\n"
    "  -p                            with -A or -M, priority of command\n"
    "                                0: important, 1: normal, 2: extra\n"
    "  -V, --version                 print r2sh version and exit\n";

#ifdef R2SH_VERSION
static const char* INFORMATION_VERSION = "Ready 2 Shell 0.4.0";
#else
static const char* INFORMATION_VERSION = "development version";
#endif /* R2SH_VERSION */

#endif /* _INFORMATION_H */
