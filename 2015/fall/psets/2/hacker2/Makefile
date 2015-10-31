#
# Makefile
#
# Computer Science 50
# Problem Set 2
#

all: crack-v1 crack-v2 generator

crack-v1: crack-v1.c Makefile
	clang -std=c11 -Wall -Werror -o crack-v1 crack-v1.c -lcrypt

crack-v2: crack-v2.c Makefile
	clang -std=c11 -Wall -Werror -o crack-v2 crack-v2.c -lcrypt

generator: generator.c
	clang -std=c11 -Wall -Werror -o generator generator.c -lcrypt

clean:
	rm -f *.o core crack-v1 crack-v2 generator
