CC = gcc
ARCH=$(PROC)
CFLAGS+= -Wall 
FLAC_PATH=flac-1.2.1/src/libFLAC/
CINCLUDE = \
	-Iinclude/ \
	-Iincude/FLAC/ \
	-I$(FLAC_PATH)/include/ \
	-Iflac-1.2.1/include/

CC_SWITCHES = -Wall $(CINCLUDE)
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

RES_SOURCES=$(addprefix $(FLAC_PATH), $(SOURCES))
MAIN = flac_as3_encoder.c

MAIN_OBJ = $(MAIN:.c=.o)
OBJ = $(RES_SOURCES:.c=.o)


all: compile_swc clean

compile_swc: $(OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -swc -o $(PNAME).swc $(OBJ) $(MAIN_OBJ)

%.o: %.c
	$(CC) $(CC_SWITCHES) -c $< -o $@

clean:
#	rm -f *.o core *.bak *.a *.exe *.exe.bc *.l.bc *.achacks.* *.stackdump *.swc
