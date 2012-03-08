CC = gcc
ARCH=$(PROC)
CFLAGS+= -Wall -O3
LIB_FLAC_PATH=flac121/libFlac/
CINCLUDE = \
	-I$(LIB_FLAC_PATH)include/ \
	-I$(LIB_FLAC_PATH) \
	-Iflac121/include/ 

CC_SWITCHES = -Wall -O3 $(CINCLUDE)
DEFS= -DHAVE_CONFIG_H

PNAME = flac

SOURCES=\
	bitmath.c \
	bitreader.c \
	bitwriter.c \
	cpu.c \
	crc.c \
	fixed.c \
	float.c \
	format.c \
	lpc.c \
	md5.c \
	memory.c \
	metadata_iterators.c \
	metadata_object.c \
	stream_decoder.c \
	stream_encoder.c \
	stream_encoder_framing.c \
	window.c

RES_SOURCES=$(addprefix $(LIB_FLAC_PATH), $(SOURCES))
MAIN = flac_as3_encoder.c

MAIN_OBJ = $(MAIN:.c=.o)
OBJ = $(RES_SOURCES:.c=.o)

all: compile_swc clean

compile_swc: $(OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -swc -o $(PNAME).swc $(OBJ) $(MAIN_OBJ)
	mv $(PNAME).swc ../bin/.

%.o: %.c
	$(CC) $(DEFS) $(CC_SWITCHES) -c $< -o $@

clean:
	rm -f $(OBJ) $(MAIN_OBJ) core *.bak *.a *.exe *.exe.bc *.l.bc *.achacks.* *.stackdump
