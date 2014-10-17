#
# Makefile
#
# Computer Science 50
# Problem Set 6
#


# compiler to use
CC = gcc

# flags to pass compiler
CFLAGS = -O2 -std=c99 -Werror

# name for executable
EXE = speller

# space-separated list of header files
HDRS = dictionary.h

# space-separated list of source files
SRCS = speller.c dictionary.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# dependencies 
$(OBJS): $(HDRS)

# housekeeping
clean:
	rm -f core $(EXE) *.o

