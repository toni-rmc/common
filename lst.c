/***********************
   @Author: Toni Romic
***********************/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "nlist.h"

#define HASHSIZE 1024 /* hash table size */

static Nlist *hashtab[HASHSIZE];    /* pointer table */

char *strdup_(char *);
static unsigned hash(char*, char *);

/* lookup:  look for (fname, line) in hashtab. */
Nlist *lookup(char *fname, char *line)
{
    Nlist *np;
    for (np = hashtab[hash(fname, line)]; np != NULL; np = np->next)
        if ((strcmp(line, np->line) == 0) && (strcmp(fname, np->filename) == 0))
            return np;      /* found */
    return NULL;            /* not found */
}

/* install:  put (fname, line) in hashtab, if object already
    exists increment lncnt. */
Nlist *install(char *fname, char *line)
{
    Nlist *np;
    unsigned hashval;

    if ((np = lookup(fname, line)) == NULL) { /* not found */
        np = malloc(sizeof(*np));
        if (np == NULL || (np->filename = strdup_(fname)) == NULL ||
                            (np->line = strdup_(line)) == NULL)
            return NULL;
        hashval = hash(fname, line);
        np->next = hashtab[hashval];
        np->lncnt = 1;
        hashtab[hashval] = np;
    } else       /* already there */
        np->lncnt++; /* increment line counter */
    return np;
}

/* hash:  form hash value for string fname and string line. */
static unsigned hash(char *fname, char *line)
{
    unsigned hashval;
    for (hashval = 0; *fname != '\0'; ++fname)
        hashval = *fname + 31 * hashval;
    for ( ; *line != '\0'; ++line)
        hashval = *line + 31 * hashval;
    return hashval % HASHSIZE;
}

/* uninst:  performs lookup on fname and line, decrement
    line counter (lnctr), if it is 0 delete node entirely,
    if is bigger than 0 exit. */
void uninst(char *fname, char *line)
{
    unsigned hashval;
    struct nlist *np1, *np2;

    hashval = hash(fname, line);
    for (np1 = hashtab[hashval], np2 = NULL;
        np1 != NULL; np2 = np1, np1 = np1->next) {
        if ((strcmp(line, np1->line) == 0) && (strcmp(fname, np1->filename) == 0)) {
            if (--np1->lncnt > 0) /* decrement and check lnct */
                return;
            if (np2 == NULL) /* at the begening */
                hashtab[hashval] = np1->next;
            else /* in the midle or at the end */
                np2->next = np1->next;
            free(np1->filename); /* free filename */
            free(np1->line); /* free line */
            free(np1); /* free node */
            return;
        }
    }
}

/* strdup_:  makes a duplicate of s. */
char *strdup_(char *s)
{
    char *p;
    p = (char *) malloc(strlen(s) + 1);
    if (p != NULL)
        strcpy(p, s);
    return p;
}

