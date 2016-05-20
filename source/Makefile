#
# This makefile is part of the atxos project.
# It requires the AVR Toolchain: 
# http://www.atmel.com/tools/atmelavrtoolchainforlinux.aspx
#

ARCH=atxmega128a1
CC=avr-gcc
CFLAGS=-g -mmcu=$(ARCH) -std=gnu99
TARGET=atxos
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
RM=rm -f
.PHONY=all clean

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
