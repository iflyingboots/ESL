SHELL = /bin/sh

BASE_TOOLCHAIN := /data/usr/local/share
BASE_COMPILER := $(BASE_TOOLCHAIN)/c6000
BASE_SABIOS := $(BASE_TOOLCHAIN)/tidspbios/bios_5_41_00_06
BASE_BSL := $(BASE_COMPILER)

CC := cl6x
TCONF := tconf

# DSPLINK := /usr/local/share/bbframework/platform/beagle-linux/tools/dsplink_linux_1_65_00_03
DSPLINK := /data/usr/local/share/bbframework/platform/beagle-linux/tools/dsplink_linux_1_65_00_03


#   ----------------------------------------------------------------------------
#   Project name.
#   This variable can be used to set all source/BIOS/object filenames
#   For clarity, in this example it sets only the .OUT .CMD and .MAP filenames
#   ----------------------------------------------------------------------------
PROJNAME := pool_notify

#   ----------------------------------------------------------------------------
#   Global variables for building mpcsxfer
#   Note that common debug, compiler and linker flags are already
#   set in the lower part of this Makefile
#   CSRCS are for C-source files, ASRCS for asm and BIOSTCF is the BIOS TCF file
#   ----------------------------------------------------------------------------
DEBUG :=
CFLAGS := -I$(BASE_BSL)/dsp/include -DP__ROFILE
LDFLAGS := -i$(BASE_BSL)/dsp/lib
CSRCS := task.c pool_notify_config.c main.c
ASRCS :=
BIOSTCF := pool_notify.tcf
OBJS :=
LIBS := -ldsplink.lib -ldsplinkpool.lib -ldsplinkmpcs.lib -ldsplinkmplist.lib -ldsplinkmsg.lib -ldsplinknotify.lib -ldsplinkringio.lib -ldsplinkdata.lib
CMDS := $(PROJNAME).cmd
MAPFILE := $(PROJNAME).map
BIN := $(PROJNAME).out

#   ----------------------------------------------------------------------------
#   DSP/BIOS generated files
#   ----------------------------------------------------------------------------
BIOSSRC := $(BIOSTCF:%.tcf=%cfg.s62) $(BIOSTCF:%.tcf=%cfg_c.c)
BIOSCMD := $(BIOSTCF:%.tcf=%cfg.cmd)

#   ----------------------------------------------------------------------------
#   Adding DSP/BIOS generated files to the sources and the linker variables
#   ----------------------------------------------------------------------------
CSRCS += $(BIOSTCF:%.tcf=%cfg_c.c)
ASRCS += $(BIOSTCF:%.tcf=%cfg.s62)
CMDS += $(BIOSCMD)

#   ----------------------------------------------------------------------------
#   DSP/BIOS common flags
#   ----------------------------------------------------------------------------
BFLAGS := -Dconfig.importPath="$(BASE_SABIOS)/packages; \
          $(BASE_SABIOS)/packages/ti/bios;              \
          $(DSPLINK)/dsp/inc/DspBios;                   \
          $(DSPLINK)/dsp/inc/DspBios/5.XX/OMAP3530;  \
          $(DSPLINK)/dsp/inc/DspBios/5.XX;./"

#   ----------------------------------------------------------------------------
#   All common compiler and linker flags
#   ----------------------------------------------------------------------------
ALL_CFLAGS := -q -pdr -pdv -pden -ml3 -mv6400+ --disable:sploop $(CFLAGS)
ALL_LDFLAGS := -z -c -q -x $(LDFLAGS)

#   ----------------------------------------------------------------------------
#   Compiler and Linker options for Debug
#   ----------------------------------------------------------------------------
CFLAGS_D := -g -d"_DEBUG" --no_compress $(DEBUG) $(ALL_CFLAGS)
OBJDIR_D := Debug
BINDIR_D := $(OBJDIR_D)
OBJS_D := $(CSRCS:%.c=$(OBJDIR_D)/%.obj) $(ASRCS:%.s62=$(OBJDIR_D)/%.obj)
LIBS_D := $(LIBS)
MAPFILE_D := $(OBJDIR_D)/$(MAPFILE)


#--gcc -pr -os -on=2 -s -op=3 -al -mw --stack=0x3000
#-I${COMPILER_PATH}/../include -I${SYSROOT}/include -I${SYSROOT}/platform/${PLATFORM}/include


#   ----------------------------------------------------------------------------
#   Compiler and Linker options for Release
#   ----------------------------------------------------------------------------
CFLAGS_R := -o3 $(ALL_CFLAGS)
OBJDIR_R := Release
BINDIR_R := $(OBJDIR_R)
OBJS_R := $(CSRCS:%.c=$(OBJDIR_R)/%.obj) $(ASRCS:%.s62=$(OBJDIR_R)/%.obj)
LIBS_R := $(LIBS)
MAPFILE_R := $(OBJDIR_R)/$(MAPFILE)

#   ----------------------------------------------------------------------------
#   Compiler common include directories
#   ----------------------------------------------------------------------------
CINCLUDES := -I$(DSPLINK)/dsp/inc                             \
             -I$(DSPLINK)/dsp/inc/DspBios                     \
             -I$(DSPLINK)/dsp/inc/DspBios/5.XX                \
             -I$(DSPLINK)/dsp/inc/C64XX                       \
             -I$(DSPLINK)/dsp/inc/DspBios/5.XX/OMAP3530		 \
             -I$(BASE_SABIOS)/packages/ti/bios/include        \
             -I$(BASE_COMPILER)/include                       \
             -I$(BASE_SABIOS)/packages/ti/rtdx/include/c6000  \
             -I$(BASE_SABIOS)/packages/ti/psl/include         \
              -I./

#             -I$(DSPLINK)/dsp/export/INCLUDE/DspBios/OMAPL1XX \
#             -I$(DSPLINK)/dsp/inc/DspBios/6.XX/OMAPL1XXGEM    \
#   ----------------------------------------------------------------------------
#   Compiler common symbol definitions
#   ----------------------------------------------------------------------------

DEFS := 	-DHOSTED_IO 				\
		-DMAX_DSPS=1                            \
		-DMAX_PROCESSORS=2                      \
		-DID_GPP=1                              \
		-DOMAP3530                              \
		-DOMAP3530_PHYINTERFACE=SHMEM_INTERFACE \
		-DPROC_COMPONENT                        \
		-DPOOL_COMPONENT                        \
		-DNOTIFY_COMPONENT                      \
		-DMPCS_COMPONENT                        \
		-DRINGIO_COMPONENT                      \
		-DMPLIST_COMPONENT                      \
		-DMSGQ_COMPONENT                        \
		-DMSGQ_ZCPY_LINK                        \
		-DCHNL_COMPONENT                        \
		-DCHNL_ZCPY_LINK                        \
		-DZCPY_LINK                             \
		-DPROCID=0                              \
		-dPHYINTERFACE=SHMEM_INTERFACE          \
		-DDSP_SWI_MODE                          \
		-DARCH_OMAP




#   ----------------------------------------------------------------------------
#   Linker common library search path
#   ----------------------------------------------------------------------------
LDINCLUDES := -i$(BASE_COMPILER)/lib                      \
              -i$(BASE_SABIOS)/packages/ti/bios/lib       \
              -i$(BASE_SABIOS)/packages/ti/rtdx/lib/c6000 \
              -i$(BASE_SABIOS)/packages/ti/psl/lib

#   ----------------------------------------------------------------------------
#   Linker library search path for Debug
#   ----------------------------------------------------------------------------
#LDINCLUDES_D := $(LDINCLUDES) -i$(DSPLINK)/dsp/export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/DEBUG/
# If the DSP/Link was rebuilt by the user, replace the line above
# with the one below to use the updated libraries
LDINCLUDES_D := $(LDINCLUDES) -i$(DSPLINK)/dsp/BUILD/OMAP3530_0/EXPORT/DEBUG/

#   ----------------------------------------------------------------------------
#   Linker library search path for Release
#   ----------------------------------------------------------------------------
#LDINCLUDES_R := $(LDINCLUDES) -i$(DSPLINK)/dsp/export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/RELEASE/
# If the DSP/Link was rebuilt by the user, replace the line above
# with the one below to use the updated libraries
LDINCLUDES_R := $(LDINCLUDES) -i$(DSPLINK)/dsp/BUILD/OMAP3530_0/EXPORT/RELEASE/

#   ----------------------------------------------------------------------------
#   Compiler and Linker procedure
#   From this point and on changes are highly unlikely.
#   - Be careful if changing the order of the linker object and library files
#   - The DSP/BIOS TCONF build needs to come before everything else
#   - The clean rule also removes all DSP/BIOS generated files
#   ----------------------------------------------------------------------------
.PHONY: all
all: $(BIOSSRC) Debug Release

$(BIOSSRC): $(BIOSTCF)
	@echo Building DSP/BIOS file...
	@$(BASE_TOOLCHAIN)/xcdtools/xdctools_3_16_01_27/$(TCONF) $(BFLAGS) $< 0

#   ----------------------------------------------------------------------------
#   Building Debug...
#   ----------------------------------------------------------------------------
.PHONY: Debug
Debug: $(BINDIR_D)/$(BIN)

$(BINDIR_D)/$(BIN) : $(OBJS_D)
	@echo Compiling Debug...
	@$(BASE_COMPILER)/bin/$(CC) $(ALL_LDFLAGS) $(LDINCLUDES_D) -o$@ $(OBJS_D) $(LIBS_D) -m$(MAPFILE_D) $(CMDS)

$(OBJDIR_D)/%.obj : %.c
	@$(BASE_COMPILER)/bin/$(CC) $(ALL_DEBUG) $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

$(OBJDIR_D)/%.obj : %.s62
	@$(BASE_COMPILER)/bin/$(CC) $(ALL_DEBUG) $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

#   ----------------------------------------------------------------------------
#   Building Release...
#   ----------------------------------------------------------------------------
.PHONY: Release
Release: $(BINDIR_R)/$(BIN)

$(BINDIR_R)/$(BIN) : $(OBJS_R)
	@echo Compiling Release...
	@echo $(BIOSSRC) $(BIOSTCF)
	@$(BASE_COMPILER)/bin/$(CC) $(ALL_LDFLAGS) $(LDINCLUDES_R) -o$@ $(OBJS_R) $(LIBS_R) -m$(MAPFILE_R) $(CMDS)

$(OBJDIR_R)/%.obj : %.c
	@$(BASE_COMPILER)/bin/$(CC) $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<

$(OBJDIR_R)/%.obj : %.s62
	@$(BASE_COMPILER)/bin/$(CC) $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<

.PHONY: clean
clean:
	@rm -f *.cdb
	@rm -f *cfg.*
	@rm -f *cfg_c.*
	@rm -f ./$(OBJDIR_D)/*
	@rm -f ./$(OBJDIR_R)/*

.PHONY: send
send:
	@echo "Sending '$(BINDIR_D)/$(BIN)' to beagle board (DEBUG MODE) ..."
	scp $(BINDIR_D)/$(BIN) root@192.168.0.202:/home/root/.


