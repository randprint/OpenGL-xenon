ifeq ($(strip $(DEVKITXENON)),)
$(error "Please set DEVKITXENON in your environment. export DEVKITXENON=<path to>devkitPPC")
endif


include $(DEVKITXENON)/rules
 
export TOPDIR	:=	$(CURDIR)
 
export LIBGL_MAJOR	:= 1
export LIBGL_MINOR	:= 0
export LIBGL_PATCH	:= 11

export VERSTRING	:=	$(LIBGL_MAJOR).$(LIBGL_MINOR).$(LIBGL_PATCH)


#default: release
default: all

#all: release dist
all: xenon-release
	
release: include/LIBGLversion.h

xenon-release:
	$(MAKE) -C source PLATFORM=xenon BUILD=xenon_release

#debug: nds-debug gba-debug cube-debug wii-debug
debug: 

#clean: nds-clean gba-clean ogc-clean
clean: 
	$(MAKE) -C source clean
	
dist: include/LIBGLversion.h dist-bin dist-src

distribute/$(VERSTRING):
	@[ -d $@ ] || mkdir -p $@

include/LIBGLversion.h : Makefile
	@echo "#ifndef __LIBGLVERSION_H__" > $@
	@echo "#define __LIBGLVERSION_H__" >> $@
	@echo >> $@
	@echo "#define _LIBGL_MAJOR_	$(LIBGL_MAJOR)" >> $@
	@echo "#define _LIBGL_MINOR_	$(LIBGL_MINOR)" >> $@
	@echo "#define _LIBGL_PATCH_	$(LIBGL_PATCH)" >> $@
	@echo >> $@
	@echo '#define _LIBGL_STRING "LIBGL Release '$(LIBGL_MAJOR).$(LIBGL_MINOR).$(LIBGL_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __LIBGLVERSION_H__" >> $@

#install: nds-install gba-install ogc-install
install: 
	$(MAKE) -C source install

