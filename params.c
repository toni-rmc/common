/****
    * common:  compare files FILE1 and FILE2 line by line, prints out unique lines and common lines.
    * Copyright (C) 2011  Toni Romic
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
****/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "defs.h"

void print_help_message(void);
void print_version(void);

char *handle_arguments(int argc, char *argv[], char **f1, char **f2, int *arg_mode)
{
    int i = 0; /* index used to move trough arguments */
    char *c;   /* one character in argument string */
    void *p;   /* for compiler satisfaction */
    static char *delimiter = NULL;    /* return this output delimiter, if any */

    /* number of characters in output delimiter switch */
    #define OUTPUT_DEL_SWITCH_NAME 17

    while (++i < argc)
    {
        /* check for each argument does it start
           with one, two or without a dash: */
        if ((*argv[i] == '-') && (*(argv[i]+1) == '-'))
        {
            /* argument starts with '--', it's a multiple character switch */
            char *swtch = argv[i]+2; /* point to switch name without dashes */
            if (strcmp("help", swtch) == 0)
            {
                print_help_message();
            }
            else if (strcmp("version", swtch) == 0)
            {
                print_version(); /* print version and other information */
            }
            else if (strcmp("trim", swtch) == 0)
            {
                *arg_mode |= TRIM;  /* trim blanks from the end of lines */
            }
            else if (strcmp("no-warn", swtch) == 0)
            {
                *arg_mode |= NO_WARN; /* don't print warnings */
            }
            else if (strncmp("output-delimiter=", swtch, OUTPUT_DEL_SWITCH_NAME) == 0)
            {
                if (!swtch[OUTPUT_DEL_SWITCH_NAME])
                {
                    /* switch value not specified */
                    fprintf(stderr, "%s: empty `--output-delimiter` not allowed\n", program);
                    exit(0); /* exit the program */
                }
                /* skip switch name and '=' and take delimiter value: */
                delimiter = swtch + OUTPUT_DEL_SWITCH_NAME;
            }
            else
            {
                /* unknown multiple character switch found */
                (*arg_mode & NO_WARN) ? :fprintf(stderr, "warning:\tunknown switch --%s\n", swtch);
            }
        }
        else if (*argv[i] == '-')
        {
            /* starts with one '-', it's a single character switch: */
            for (c = ++argv[i]; *c; c++) /* skip '-' in switch name */
            {
                switch (*c)
                {
                    case '1':              /* don't print 1st column */
                        *arg_mode |= NO_C1;
                    break;
                    case '2':              /* don't print 2nd column */
                        *arg_mode |= NO_C2;
                    break;
                    case '3':              /* don't print 3rd column */
                        *arg_mode |= NO_C3;
                    break;
                    default: /* if unknown switch is found and --no-warn is not set, print a warning message */
                        (*arg_mode & NO_WARN) ? :fprintf(stderr, "warning:\tunknown switch -%c\n", *c);
                    break;
                }
            } /* end of FOR loop */
        }
        else
        {
            /* no dash - not a switch, it's a file name:*/
            /* if f1 not set, set it, else set f2:  */
            if (*f1 == NULL)
                *f1 = argv[i];
            else if (*f2 == NULL)
                *f2 = argv[i];
            /* if first two file names specified are
               set, ignore what might be other file
               names but keep scanning for switches */
        }
    } /* end of WHILE loop */

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
  --no-warn  don't print out warnings\n\
  --output-delimiter=STR  separate columns with STR\n\
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
