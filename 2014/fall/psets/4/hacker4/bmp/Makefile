#
# Makefile
#
# Computer Science 50
# Problem Set 4
#

all: resize whodunit

resize: resize.c Makefile
	clang -std=c99 -O0 -Wall -Werror -o resize resize.c

whodunit: whodunit.c Makefile
	clang -std=c99 -O0 -Wall -Werror -o whodunit whodunit.c

clean:
	rm -f *.o core resize whodunit
