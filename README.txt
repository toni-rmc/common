common is a software much like well known Unix/Linux comm. No sorting needed.
It's usage is the same as comm:

> common FILE1 FILE2

It prints out lines specific to FILE1 in the first column, lines specific to FILE2 in the
second column, and lines common to both files in the third column.

common has one major advantage over comm, and that is; it does NOT need files to be sorted
before they are fed to it to work properly.
You can freely use common on unsorted files and it will work as expected.
common will not perform any sorting, not on the files and not on the output.

To list available switches and usage information, type:

> common --help

To build this software first position yourself in src folder,
which is in the same directory as this file:

> cd src

Than simply run make:

> make

or:

> make all

This will create bin folder in a directory where this file is found and it will
contain executable.
I have tested building and runing this code only on Windows but it should work well on
Unix and GNU/Linux because it does not contain any platform specific code.
Makefile invokes gcc compiler so you should have it installed on your system.

There is no install command in the Makefile at this point so if you want
to run this program just copy and paste executable manualy from the bin
folder in this directory in /usr/local/bin if you are on Unix or GNU/Linux,
or in %PROGRAMFILES% if you are on Windows platform.

To clean object files, dependency files and executable as well as out and bin folders type:

> cd src
> make clean

Toni Romic
email: toni.rmc@gmail.com
