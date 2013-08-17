/***********************
   @Author: Toni Romic
***********************/




typedef struct nlist {       /* table entry: */
    int lncnt;            /* line counter */
    char *filename;       /* file name */
    char *line;           /* line in file */
    struct nlist *next;   /* next entry in chain */
} Nlist;

Nlist *lookup(char *, char *);
Nlist *install(char *, char *);
void uninst(char *, char *);
