#
# Makefile
#

CC		:= gcc
AR 		:= ar

TARGET	:= libtinycv.a

OBJECTS := $(patsubst %.c, %.o, $(wildcard *.c))

# 
LIBPATH = .

CFLAGS		:= -I. -I$(LIBPATH) 
CFLAGSEX	:=
LIBRARIES	:= 

.PHONY: all clean

all: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(AR) cr $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGSEX) -c -o $@ $< $(LIBRARIES)

clean:
	rm -f *.o $(TARGET)
	
debug:
	make clean
	make CFLAGSEX="-O0 -g"

mips:
	make clean
	make CC=mips-openwrt-linux-gcc AR=mips-openwrt-linux-ar CFLAGSEX="-DMIPS=1"
