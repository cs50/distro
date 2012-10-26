#
# Makefile
#
# Computer Science 50
# Problem Set 6
#


# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -O0 -Qunused-arguments -std=c99 -Wall -Werror

# dump
DUMP = dump
DUMP_SRCS = dump.c forest.c huffile.c tree.c
DUMP_OBJS = $(DUMP_SRCS:.c=.o)

# huff
HUFF      = huff
HUFF_SRCS = forest.c huff.c huffile.c tree.c
HUFF_OBJS = $(HUFF_SRCS:.c=.o)

# puff
PUFF      = puff
PUFF_SRCS = forest.c huffile.c puff.c tree.c
PUFF_OBJS = $(PUFF_SRCS:.c=.o)

# space-separated list of header files
HDRS = forest.h huffile.h tree.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS = -lm

# targets
$(DUMP): $(DUMP_OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(DUMP_OBJS) $(LIBS)
$(HUFF): $(HUFF_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(HUFF_OBJS) $(LDLIBS)

$(PUFF): $(PUFF_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(PUFF_OBJS) $(LDLIBS)

# housekeeping
clean:
	rm -f core $(DUMP) $(HUFF) $(PUFF) *.o
