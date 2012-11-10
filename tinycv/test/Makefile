#
# Makefile
#

CC		:= gcc

OBJECTS := $(patsubst %.c, %.o, $(wildcard *.c))

TARGETS	:= $(patsubst %.o, %, $(OBJECTS))

# 
LIBPATH = ..

CFLAGS		:= -I. -I$(LIBPATH) -L$(LIBPATH)
CFLAGSEX	:=
LIBRARIES	:= -ltinycv -lm

.PHONY: all clean

all: $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGSEX) -o $(patsubst %.o, %, $@) $< $(LIBRARIES)

clean:
	rm -f *.o $(TARGETS)

debug:
	make clean
	make CFLAGSEX="-O0 -g"
	
mips:
	make clean
	make CC=mips-openwrt-linux-gcc CFLAGSEX="-DMIPS=1"
