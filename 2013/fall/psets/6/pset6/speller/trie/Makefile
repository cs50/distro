#
# Makefile
#
# Computer Science 50
# Problem Set 6
#


# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -funroll-loops -O3 -Qunused-arguments -std=c99 -Wall -Werror

# name for executable
EXE = speller

# space-separated list of header files
HDRS = dictionary.h

# space-separated list of source files
SRCS = speller.c dictionary.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# dependencies 
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
