
#libdragon path setup
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
MKSPRITEPATH= $(ROOTDIR)/bin/mksprite
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
#link to pull request: https://github.com/DragonMinded/libdragon/issues/57, make sure -lm is before -lc
#when linking in math lib, etc...
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lm -lc -ldragonsys -Tn64.ld
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 24M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 24M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
endif

#your program name, if you get something like "circular depency" ensure you don't have any
#whitespace on PROG_NAME end
PROG_NAME = 2d_roguelike#should be no whitespace here or will fail
PROG_TAG = "Clone of Unity 2D Roguelike Example"
ROM_DEPLOY_PATH = "./../2drogue_like.z64"

#export .o files to obj/ during build
O=obj

#files to compile, in order of compilation needed
OBJS=					\
		$(O)/audio.o	\
		$(O)/utils.o	\
		$(O)/map.o		\
		$(O)/ui.o		\
		$(O)/input.o	\
		$(O)/player.o	\
		$(O)/enemy.o	\
		$(O)/debug.o		\
		$(O)/2d_roguelike.o    


#audio build setup
SOX = sox
SOXFLAGS = -b 16 -e signed-integer -B -r 44100
AIFF_DIR = ./res/sfx
AIFF_FILES := $(wildcard $(AIFF_DIR)/*.aiff)
PCM_DIR := ./res/sfx
PCM_TMP = $(subst $(AIFF_DIR),$(PCM_DIR),$(AIFF_FILES))
PCM_FILES := $(PCM_TMP:.aiff=.raw)

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf packed_files.dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary 
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t $(PROG_TAG) $(PROG_NAME).bin -s 1M packed_files.dfs
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS)

# build audio
$(PCM_DIR)/%.raw: $(AIFF_DIR)/%.aiff
	@mkdir -p $(PCM_DIR)
	@command -v $(SOX) >/dev/null 2>&1 || { \
	    echo >&2 'This Makefile requires the `sox` command.'; \
	    echo >&2 'Get it from http://sox.sourceforge.net/sox.html'; \
	    exit 1; \
	}
	$(SOX) $< $(SOXFLAGS) $@ remix -

#pack our spritesheet.sprite into dfs (Dragon File System)
#$(MKSPRITEPATH), 16 bits 8 column, 1 rows of sprites (56 sprites total) used
#to convert spritesheet.png into compat. Dragon format
packed_files.dfs: $(PCM_FILES)
	$(MKSPRITEPATH) 16 8 7 ./res/spritesheet.png ./res/spritesheet.sprite 
	$(MKDFSPATH) packed_files.dfs ./res/

all: $(PROG_NAME)$(ROM_EXTENSION)
	mv $(PROG_NAME).z64 $(ROM_DEPLOY_PATH)

#build any required source files
$(O)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#remove files produced during build
clean:
	rm -f *.v64 *.z64 *.elf *.o *.bin *.dfs
	rm -f $(O)/*