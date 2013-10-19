#
# Makefile
#
# Computer Science 50
# Problem Set 5
#

all: peek resize whodunit

peek: peek.c Makefile
	clang -std=c99 -O0 -Wall -Werror -o peek peek.c

resize: resize.c Makefile
	clang -std=c99 -O0 -Wall -Werror -o resize resize.c

whodunit: whodunit.c Makefile
	clang -std=c99 -O0 -Wall -Werror -o whodunit whodunit.c

clean:
	rm -f *.o core peek resize whodunit
