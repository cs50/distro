#
# Makefile
#
# David J. Malan
# malan@harvard.edu
#

demo: demo.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o demo demo.c -Lspl/lib -lcs -lm

clean:
	rm -f *.o core demo
