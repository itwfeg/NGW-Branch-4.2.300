##############################################################
#
# File : makefile
#
# Make file used for building EvolutionRTC/PngGraphics/
#
##############################################################

###################################
#defines
###################################
OS = linux

HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application
export HOBART_APP
export HOBART_OUT_DIR=$(HOBART_APP)/output


##################################
#make vars
##################################

#
#Vars for Source and object files
#
SUB_DIRS :=
C_SRCS := \
    adler32.c \
    crc32.c \
    deflate.c \
    hob2png.c \
    png.c \
    png2hob.c \
    pngerror.c \
    pngget.c \
    pngmem.c \
    pngread.c \
    pngrio.c \
    pngrtran.c \
    pngrutil.c \
    pngset.c \
    pngtrans.c \
    pngwio.c \
    pngwrite.c \
    pngwtran.c \
    pngwutil.c \
    trees.c \
    zutil.c

CPP_SRCS :=


##################################
# makefile template
##################################

include $(HOBART_APP)/hobart.mk
