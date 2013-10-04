#
# Makefile
#
# David J. Malan
# malan@harvard.edu
#

all: cursor window

cursor: cursor.c
	clang -ggdb3 -Iinclude -O0 -std=c99 -Wall -Werror -o cursor cursor.c -Llib -lcs -lm

window: window.c
	clang -ggdb3 -Iinclude -O0 -std=c99 -o window window.c -Llib -lcs -lm

clean:
	rm -f *.o core cursor window
