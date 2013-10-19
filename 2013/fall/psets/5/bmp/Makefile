#
# Makefile
#
# David J. Malan
# malan@harvard.edu
#

all: bounce button checkbox click cursor label slider text window

bounce: bounce.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o bounce bounce.c -Lspl/lib -lcs -lm

button: button.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o button button.c -Lspl/lib -lcs -lm

checkbox: checkbox.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o checkbox checkbox.c -Lspl/lib -lcs -lm

click: click.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -Wno-unused-variable -o click click.c -Lspl/lib -lcs -lm

cursor: cursor.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o cursor cursor.c -Lspl/lib -lcs -lm

label: label.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o label label.c -Lspl/lib -lcs -lm

slider: slider.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o slider slider.c -Lspl/lib -lcs -lm

text: text.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o text text.c -Lspl/lib -lcs -lm

window: window.c Makefile
	clang -ggdb3 -Ispl/include -O0 -std=c99 -Wall -Werror -o window window.c -Lspl/lib -lcs -lm

clean:
	rm -f *.o core bounce button checkbox click cursor label slider text window
