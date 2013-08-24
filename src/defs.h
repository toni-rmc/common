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





#define MAXLINE 10000    /* maximum line size */

#define COLUMN_1 0X01    /* 1st column */
#define COLUMN_2 0X02    /* 2nd column */
#define COLUMN_3 0X04    /* 3rd column */

#define NO_C1 COLUMN_1   /* skip 1st column */
#define NO_C2 COLUMN_2   /* skip 2nd column */
#define NO_C3 COLUMN_3   /* skip 3rd column */
#define NO_WARN 0X08     /* don't print warnings */
#define TRIM  0X80       /* trim end of lines from all blanks */

static const char *program = "common";
