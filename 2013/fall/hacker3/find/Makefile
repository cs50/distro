#
# Makefile
#
# Computer Science 50
# Problem Set 3
#

all: find generate

find: find.c helpers.c helpers.h
	gcc -std=c99 -Wall -Werror -o find find.c helpers.c -lcs50 -lm

generate: generate.c
	gcc -std=c99 -Wall -Werror -o generate generate.c

clean:
	rm -f *.o a.out core find generate
