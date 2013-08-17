/***********************
   @Author: Toni Romic
***********************/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "nlist.h"

#define MAXLINE 10000    /* maximum line size */

#define COLUMN_1 0X01    /* 1st column */
#define COLUMN_2 0X02    /* 2nd column */
#define COLUMN_3 0X04    /* 3rd column */

#define NO_C1 COLUMN_1   /* skip 1st column */
#define NO_C2 COLUMN_2   /* skip 2nd column */
#define NO_C3 COLUMN_3   /* skip 3rd column */
#define NO_WARN 0X08     /* don't print warnings */
#define OWR_1 0X10       /* overwrite first file */
#define OWR_2 0X20       /* overwrite second file */
#define TRIM  0X80       /* trim end of lines from all blanks */

static int arg_mode = 0; /* argument list mode */

typedef struct openFile {       /* object to store open file information */
    FILE *file;              /* pointer to open file */
    char *filename;          /* name of the file */
    int column;              /* column asociated with file */
} Openfile;

Openfile *openf(char *, int);
void closef(Openfile *);
void switchfiles(Openfile *, Openfile *);
void print_help_message(void);
int trim(char *);

int main(int argc, char *argv[])
{
    char *f1 = NULL; /* stores name of the 1st file */
    char *f2 = NULL; /* stores name of the 2nd file */
    void *p;   /* dummy, not used */
    char *c;   /* one character in argument string */
    int i = 0; /* index used to move trough arguments */

    /* begin code to handle paremeters */
    while (++i < argc) {
        if (*(argv[i]) == '-') { /* is it switch */
            for (c = ++argv[i]; *c; c++) { /* skip '-' in switch name */
                switch (*c) {
                    case '1':              /* don't print 1st column */
                        arg_mode |= NO_C1;
                    break;
                    case '2':              /* don't print 2nd column */
                        arg_mode |= NO_C2;
                    break;
                    case '3':              /* don't print 3rd column */
                        arg_mode |= NO_C3;
                    break;
                    case 's':              /* don't print warnings */
                        arg_mode |= NO_WARN;
                    break;
                    case 't':
                        arg_mode |= TRIM;  /* trim blanks from the end of lines */
                    break;
                    case 'F':              /* overwrite 1st file specified */
                        arg_mode |= OWR_1;
                        arg_mode &= ~OWR_2;
                    break;
                    case 'S':              /* overwrite 2nd file specified */
                        arg_mode |= OWR_2;
                        arg_mode &= ~OWR_1;
                    break;
                    case 'h':
                        print_help_message();
                    break;
                    default: /* if unknown switch is found and -s is not set, print warning message */
                        (arg_mode & NO_WARN) ? :fprintf(stderr, "warning:\tunknown switch -%c\n", *c);
                    break;
                }
            }
        } else { /* not a switch */
            /* if both filenames are given and if -F switch is found, overwrite first filename */
            if (f1 && f2 && (arg_mode & OWR_1))
                f1 = argv[i];
            else /* if f1 not set, set it, else if f2 not set, set it or overwrite if -S specified */
                p = (!f1) ? f1 = argv[i] : ((arg_mode & OWR_2) || !f2) ? f2 = argv[i] : f2;
        }
    } /* end code to handle parameters */

    if (f1 == NULL || f2 == NULL) { /* one or both filenames are not found */
        fprintf(stderr, "two files must be given for comparing\n");
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

    if (of == NULL || (of->filename = malloc(strlen(fname)+1)) == NULL) {
        perror("memmory alocation faliure");
        exit(ENOMEM);
    }
    if ((of->file = fopen(fname, "r")) == NULL) {
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

    while (!feof(f1->file) || !feof(f2->file)) {
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
    that switch is set to silence this column so skip printing. */
void compare_files(Openfile *file_to_read, Openfile *file_to_compare)
{
    int is_allready_compared(char *, Openfile *);
    int search_for_line(char *, Openfile *, int);

    if (feof(file_to_read->file)) /* first check to see if EOF is reached */
        return;

    char line[MAXLINE];
    int column;
    char *tab;

    do { /* skip empty lines */
        if ((fgets(line, MAXLINE, file_to_read->file)) == NULL)
            return; /* reading of line is failed */
    } while ((!trim(line)) && (!feof(file_to_read->file)));

    if (!*line)
        return;

    if (is_allready_compared(line, file_to_read))
        return; /* line is allready matched previously don't try to match it again */
    if (arg_mode & (column = search_for_line(line, file_to_compare, file_to_read->column)))
        return; /* switch is set to skip printing this column */
    switch (column) {
        case COLUMN_1:
            tab = "";
            break;
        case COLUMN_2:
            tab = "\t\t";
            break;
        case COLUMN_3:
            tab = "\t\t\t";
            break;
        default:
            tab = "_NO_TAB_";
            break;
    }
    printf("%s%s\n", tab, line);
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
        return 0; /* not found */
    else {
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

    int matched = 0;
    char line_to_compare[MAXLINE];
    long pos = ftell(file_to_compare->file); /* mark current position */
    Nlist *node = NULL;

    while (!feof(file_to_compare->file)) {
        fgets(line_to_compare, MAXLINE, file_to_compare->file); /* read lines from file to compare */
        trim(line_to_compare);
        if ((strcmp(line, line_to_compare)) == 0) { /* line is matched */
            matched++; /* increment counter of matched lines */
            if (node != NULL || (node = lookup(file_to_compare->filename, line)) != NULL) {
                /* if matched is bigger than line counter line is not yet
                   matched, otherwise allready matched line is found */
                if (matched > node->lncnt) {
                    install(file_to_compare->filename, line);
                    cmn = COLUMN_3;
                    break;
                }
            } else { /* first reading of the matched line */
                    install(file_to_compare->filename, line);
                    cmn = COLUMN_3;
                    break;
            }
        }
    }

    fseek(file_to_compare->file, pos, SEEK_SET); /* restore marked position */
    return cmn;
}

/* trim:  trims trailng '\n' character. If no_trim switch is on
than trims all last blank characters. Returns true if line still
exists after triming, false otherwise. */
int trim(char *line)
{
    int len;

    len = strlen(line) - 1;
    if (arg_mode & TRIM) {
        for ( ;len >= 0; len--) {
            if (isspace(line[len]))
                line[len] = '\0';
            else
                break;
        }
    } else {
        if (line[len] == '\n')
            line[len] = '\0';
    }
    return (*line) ? 1 : 0;
}

/* print_help_message:  name says it */
void print_help_message()
{
    printf("help\n");
    exit(0);
}
