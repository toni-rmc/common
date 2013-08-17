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
#include <ctype.h>
#include <errno.h>
#include "nlist.h"
#include "defs.h"

static int arg_mode = 0; /* argument list mode */
static char *delimiter;  /* output delimiter */

typedef struct openFile
{       /* object to store open file information */
    FILE *file;              /* pointer to open file */
    char *filename;          /* name of the file */
    int column;              /* column asociated with file */
} Openfile;

char *handle_arguments(int, char **, char **, char **, int *);
Openfile *openf(char *, int);
void closef(Openfile *);
void switchfiles(Openfile *, Openfile *);
int trim(char *);

int main(int argc, char *argv[])
{
    char *f1 = NULL; /* stores name of the 1st file */
    char *f2 = NULL; /* stores name of the 2nd file */

    delimiter = handle_arguments(argc, argv, &f1, &f2, &arg_mode);

    if (f1 == NULL || f2 == NULL)
    {
        /* one or both filenames are not found */
        fprintf(stderr, "%s: missing operand", program);
        if (f1) fprintf(stderr, " after `%s`", f1);
        fprintf(stderr, "\n");
        fprintf(stderr, "Try `%s --help` for more information.\n", program);
        return EAGAIN;
    }
    Openfile *fp1 = openf(f1, COLUMN_1);
    Openfile *fp2 = openf(f2, COLUMN_2);
    switchfiles(fp1, fp2);
    closef(fp1);
    closef(fp2);
    return 0;
}

/* openf:  opens file fname, records information in object
    returns it.If memmory allocation or opening file failes
    exit program returning apropriate status number. */
Openfile *openf(char *fname, int column)
{
    Openfile *of = malloc(sizeof(Openfile));

    if (of == NULL || (of->filename = malloc(strlen(fname)+1)) == NULL)
    {
        perror("memmory alocation faliure");
        exit(ENOMEM);
    }
    if ((of->file = fopen(fname, "rb")) == NULL)
    {
        fprintf(stderr, "can't open file:\t%s\n", fname);
        exit(ENOENT);
    }
    strcpy(of->filename, fname);
    of->column = column;
    return of;
}

/* closef:  closes file stream in ofile object,
    removes filename, deletes object. */
void closef(Openfile *ofile)
{
    fclose(ofile->file);
    free(ofile->filename);
    free(ofile);
}

/* switchfiles:  switches files in f1 and f2 to compare
    against one another. */
void switchfiles(Openfile *f1, Openfile *f2)
{
    void compare_files(Openfile *, Openfile *);

    int replace = 1;

    while (!feof(f1->file) || !feof(f2->file))
    {
        (replace) ? compare_files(f1, f2) : compare_files(f2, f1);
        replace = (replace) ? 0 : 1;
    }
}

/* compare_files:  reads one line from file stream in file_to_read.
    If line is compared before, skip comparing an exit function.
    Read lines, from last position, in file_to_compare object, if
    match is found print line in column COLUMN_3, if not print line
    in column that is stored in file_to_read object. Before printig
    check if returned COLUMN_(X) matches NO_C(X), if it does, means
    that switch is set to skip printing this column. */
void compare_files(Openfile *file_to_read, Openfile *file_to_compare)
{
    int is_allready_compared(char *, Openfile *);
    int search_for_line(char *, Openfile *, int);

    if (feof(file_to_read->file)) /* first check to see if EOF is reached */
        return;

    char line[MAXLINE];
    int column;
    char *tab;

    do
    { /* skip empty lines */
        if ((fgets(line, MAXLINE, file_to_read->file)) == NULL)
            return; /* reading of line is failed */
    } while ((!trim(line)) && (!feof(file_to_read->file)));

    if (!*line)
        return;

    if (is_allready_compared(line, file_to_read))
    {
        return; /* line is allready matched previously don't try to match it again */
    }
    if (arg_mode & (column = search_for_line(line, file_to_compare, file_to_read->column)))
    {
        return; /* switch is set to skip printing this column */
    }
    /* set delimiter, default is '\t' if not specified on the command line: */
    tab = (delimiter) ? delimiter : "\t";
    switch (column)
    {
        case COLUMN_1:
            ; /* do nothing, no delimiter before first column */
            break;
        case COLUMN_2:
            printf("%s%s", tab, tab);
            break;
        case COLUMN_3:
            printf("%s%s%s", tab, tab, tab);
            break;
        default:
            tab = "_NO_TAB_";
            break;
    }
    printf("%s\n", line);
}

/* is_allready_compared:  performs lookup on line and filename
    in ftr. If NULL is returned means that line in file to read
    is not matched earlier and return 0. If object is returned,
    means that line has been matched earlier, uninstall the line
    and return 1. On error return -1. */
int is_allready_compared(char *line, Openfile *ftr)
{
    Nlist *node;

    if ((node = lookup(ftr->filename, line)) == NULL)
    {
        return 0; /* not found */
    }
    else
    {
        uninst(ftr->filename, line);
        return 1; /* found, matched before */
    }
    return -1; /* returns on error */
}

/* search_for_line:   looks for line in file_to_compare. File is read
    from the last read position which is saved and restored at the end.
    Install (filename, line) if match is found and matched counter is
    bigger than line counter in file_to_compare, return COLUMN_3.
    Otherwise return column. */
int search_for_line(char *line, Openfile *file_to_compare, int column)
{
    int cmn = column;

    if (feof(file_to_compare->file)) /* see if EOF is reached */
        return cmn;

    static char line_to_compare[MAXLINE];
    static fpos_t pos;
    static int matched;
    static Nlist *node;

    matched = 0;
    node = NULL;

    fgetpos(file_to_compare->file, &pos); /* mark current position */
    while (!feof(file_to_compare->file))
    {
        *line_to_compare = '\0'; /* clean garbage values */
        fgets(line_to_compare, MAXLINE, file_to_compare->file); /* read lines from file to compare */
        trim(line_to_compare);
        if ((strcmp(line, line_to_compare)) == 0)
        {
            /* line is matched, increment
                counter of matched lines: */
            matched++;
            if (node != NULL || (node = lookup(file_to_compare->filename, line)) != NULL)
            {
                /* if matched is bigger than line counter line is not yet
                   matched, otherwise allready matched line is found */
                if (matched > node->lncnt)
                {
                    install(file_to_compare->filename, line);
                    cmn = COLUMN_3;
                    break;
                }
            }
            else
            {
                /* first reading of the matched line */
                install(file_to_compare->filename, line);
                cmn = COLUMN_3;
                break;
            }
        }
    } /* end of WHILE loop */
    fsetpos(file_to_compare->file, &pos); /* restore marked position */
    return cmn;
}

/* trim:  trims trailng '\n' character. If no_trim switch is on
than trims all last blank characters. Returns true if line still
exists after triming, false otherwise. */
int trim(char *line)
{
    int len;

    len = strlen(line) - 1;
    if (arg_mode & TRIM)
    {
        for ( ;len >= 0; len--)
        {
            if (isspace(line[len]))
                line[len] = '\0';
            else
                break;
        }
    }
    else
    {
        if (line[len] == '\n')
            line[len] = '\0';
    }
    return (*line) ? 1 : 0;
}
