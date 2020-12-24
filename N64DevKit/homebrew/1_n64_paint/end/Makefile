
#libdragon path setup
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
MKSPRITEPATH= $(ROOTDIR)/bin/mksprite
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lc -lm -ldragonsys -Tn64.ld
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
endif

#your program name, if you get something like "circular depency ensure you don't have any
#whitespace on PROG_NAME end
PROG_NAME = n64_paint#should be no whitespace here or will fail
PROG_TAG = "N64 Paint!"
ROM_DEPLOY_PATH = "./../n64_paint.z64"

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf packed_files.dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t $(PROG_TAG) $(PROG_NAME).bin -s 1M packed_files.dfs
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)

$(PROG_NAME).elf : $(PROG_NAME).o
	$(LD) -o $(PROG_NAME).elf $(PROG_NAME).o $(LINK_FLAGS)

#pack our icons.sprite into dfs (Dragon File System)
#$(MKSPRITEPATH), 16 bits 1 column, 17 rows of sprites (17 sprites total) used
#to convert spritesheet.png into compat. Dragon format
packed_files.dfs:
	$(MKSPRITEPATH) 16 1 31 ./res/spritesheet.png ./res/spritesheet.sprite 
	$(MKDFSPATH) packed_files.dfs ./res/

all: $(PROG_NAME)$(ROM_EXTENSION)
	mv $(PROG_NAME).z64 $(ROM_DEPLOY_PATH)

clean:
	rm -f *.v64 *.z64 *.elf *.o *.bin *.dfs
