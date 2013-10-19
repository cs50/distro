#
# Makefile
#
# David J. Malan
# malan@harvard.edu
#

slider: slider.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o slider slider.c -Lspl/lib -lcs -lm

clean:
	rm -f *.o core slider
