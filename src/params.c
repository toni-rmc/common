/**
  * common:  compare files FILE1 and FILE2 line by line, prints out unique lines and common lines.
  * Copyright (C) 2011, 2013 Toni Romic
  *
  * This file is part of common.
  *
  * common is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * common is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with common.  If not, see <http://www.gnu.org/licenses/>.
**/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "defs.h"

void print_help_message(void);
void print_version(void);

/* Long options: */
static struct option long_options[] =
{
    { .name = "help",             .has_arg = no_argument,       .flag = 0, .val = 'h' },
    { .name = "version",          .has_arg = no_argument,       .flag = 0, .val = 'v' },
    { .name = "trim",             .has_arg = no_argument,       .flag = 0, .val = 't' },
    { .name = "silent",           .has_arg = no_argument,       .flag = 0, .val = 's' },
    { .name = "column-delimiter", .has_arg = required_argument, .flag = 0, .val = 'd' },
    { 0, 0, 0, 0 }
};

char *handle_arguments(int argc, char *argv[], char **f1, char **f2, int *arg_mode)
{
    int i, c, indexptr;

    /* Value to return: */
    static char *delimiter = NULL;

    while ((c = getopt_long(argc, argv, "123", long_options, &indexptr)) != -1)
    {
        switch (c)
        {
            case '1':
                /* Don't print 1st column. */
                *arg_mode |= NO_C1;
                break;

            case '2':
                /* Don't print 2nd column. */
                *arg_mode |= NO_C2;
                break;

            case '3':
                /* Don't print 3rd column. */
                *arg_mode |= NO_C3;
                break;

            case 't':
                /* Trim blanks from an end of lines. */
                *arg_mode |= TRIM;
                break;

            case 's':
                /* Suppress warnings. */
                *arg_mode |= NO_WARN;
                opterr = 0;
                break;

            case 'd':
                /* Take delimiter argument. */
                delimiter = optarg;
                break;

            case 'v':
                /* Print version and other information and exit. */
                print_version();
                break;

            case 'h':
                /* Print help message and exit. */
                print_help_message();
                break;

            case '?':
                /* Some error in parsing arguments occurred
                e.g., missing argument or unknown switch.
                Do nothing. getopt_long() will print the
                error on it's own if NO_WARN flag is not set. */
                break;

            default:
                /* Something totaly unexpected. */
                abort();
                break;
        }
    } /* end of WHILE loop */

    /* Look for non-switch arguments.
    Two valid file names expected */
    for (i = optind; i < argc; i++)
    {
        if (*f1 == NULL)
            *f1 = argv[i];
        else if (*f2 == NULL)
            *f2 = argv[i];
        else
            break;
    }

    return delimiter;
}

/* print_help_message:  print help message */
void print_help_message()
{
    printf("Usage: common [OPTION]... FILE1 FILE2\n\
Compare sorted files FILE1 and FILE2 line by line.\n\n\
With no options, produce three-column output.  Column one contains\n\
lines unique to FILE1, column two contains lines unique to FILE2,\n\
and column three contains lines common to both files.\n\n\
  -1              suppress column 1 (lines unique to FILE1)\n\
  -2              suppress column 2 (lines unique to FILE2)\n\
  -3              suppress column 3 (lines that appear in both files)\n\n\
  --trim     trim spaces from the end of the line before comparing\n\
  --silent   suppress warnings\n\
  --column-delimiter[=]<STR>  separate columns with <STR>\n\
      --help     display this help and exit\n\
      --version  output version information and exit\n\n\
Examples:\n\
  common -12 file1 file2  Print only lines present in both file1 and file2.\n\
  common -3 file1 file2  Print lines in file1 not in file2, and vice versa.\n\n\
Report common bugs to toni.rmc@gmail.com\n");
    exit(0);
}

/* print_version:  print version and other information */
void print_version()
{
    printf("common 1.0\nCopyright (C) 2011 Toni Romic.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n\
Written by Toni Romic.\n");
    exit(0);
}
