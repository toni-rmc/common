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





typedef struct nlist
{       /* table entry: */
    int lncnt;            /* line counter */
    char *filename;       /* file name */
    char *line;           /* line in file */
    struct nlist *next;   /* next entry in chain */
} Nlist;

Nlist *lookup(char *, char *);
Nlist *install(char *, char *);
void uninst(char *, char *);
