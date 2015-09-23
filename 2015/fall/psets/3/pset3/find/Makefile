#
# Makefile
#
# Computer Science 50
# Problem Set 3
#

all: find generate

find: find.c helpers.c helpers.h
	clang -std=c11 -Wall -Werror -o find find.c helpers.c -lcs50

generate: generate.c
	clang -std=c11 -Wall -Werror -o generate generate.c

clean:
	rm -f *.o core find generate
