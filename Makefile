#
# File          : Makefile
# Description   : Build file for CMPSC443 Project-3

# Environment Setup
LIBDIRS=-L. 
CC=gcc 
CFLAGS=-c $(INCLUDES) -g -Wall
LINK=gcc -g
LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

# Setup builds
TARGETS=cmpsc443-p3
LIBS=-lgcrypt 

# Project Protections
p3 : $(TARGETS)

cmpsc443-p3 : cmpsc443-refmon.o cmpsc443-lattice.o cmpsc443-list.o cmpsc443-utilities.o
	$(LINK) $(LDFLAGS) cmpsc443-refmon.o cmpsc443-lattice.o cmpsc443-list.o \
		cmpsc443-utilities.o $(LIBS) -o $@

clean:
	rm -f *.o *~ $(TARGETS)