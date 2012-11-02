#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITXENON)),)
$(error "Please set DEVKITXENON in your environment. export DEVKITXENON=<path to>devkitPPC")
endif

include $(DEVKITXENON)/rules

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# LIBDIR is where the built library will be placed
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		:=	xenon_release
SOURCES		:=	.
INCLUDES	:=	../include
DATA		:=
LIBDIR		:=	lib

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	= -g -O3 -DXENON -ffast-math -fomit-frame-pointer -funroll-loops -ffunction-sections -fdata-sections -fno-tree-vectorize -fno-tree-slp-vectorize -ftree-vectorizer-verbose=1 -flto -fuse-linker-plugin -maltivec -mabi=altivec -fno-pic -mpowerpc64 -mhard-float -Wall -mcpu=cell -mtune=cell -m32 -fno-pic -mpowerpc64 -u read -u _start -u exc_base $(MACHDEP) $(INCLUDE) -DLIBXENON
CXXFLAGS	=	$(CFLAGS)

ASFLAGS	:=	-g

ifneq ($(BUILD),debug)
export LIBGL	:=	$(LIBDIR)/$(PLATFORM)/libgl.a
else
export LIBGL	:=	$(LIBDIR)/$(PLATFORM)/libgl.a
CFLAGS	+=	-DGLOB_DEBUG
endif

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export TOPDIR ?= $(CURDIR)/..


export DEPSDIR := $(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))


export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/../$(BUILD) \
			-I$(LIBXENON_INC)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/../lib) \
										-L$(LIBXENON_LIB)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr xenon_release $(LIBDIR)

all: $(LIBGL)


install:
	mkdir -p $(DEVKITXENON)/usr/include/gl
	mkdir -p $(DEVKITXENON)/usr/include/gl/shaders
	cp -f ../include/*.h $(DEVKITXENON)/usr/include/gl
	cp -f ../include/shaders/*.h $(DEVKITXENON)/usr/include/gl/shaders
	cp -f ../include/shaders/*.hlsl $(DEVKITXENON)/usr/include/gl/shaders
	cp -f ./xenon_release/lib/xenon/libgl.a $(DEVKITXENON)/usr/lib/libgl.a	
	
#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(LIBGL)	:	$(OFILES) $(LIBDIR)/$(PLATFORM)
	@rm -f "$(LIBGL)"
	@$(AR) rcs "$(LIBGL)" $(OFILES)
	@echo built ... $(notdir $@)

$(LIBDIR)/$(PLATFORM):
	mkdir  -p $(LIBDIR)/$(PLATFORM)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

