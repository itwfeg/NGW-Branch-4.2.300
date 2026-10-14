##############################################################
#
# File : hobartarch.mk
#
# Make file template for Hobart application targeting
# specific architectures
#
##############################################################

##############################################################
#
# cross compiler header and linker fixes
#
##############################################################

ifeq ($(CROSS_COMPILE),arm-linux-gnueabi-)
	FLOAT_ABI_FLAGS=-mfloat-abi=softfp
endif

ifeq ($(CROSS_COMPILE),arm-linux-gnueabihf-)
	FLOAT_ABI_FLAGS=-mfloat-abi=hard
endif

ifeq ($(CROSS_COMPILE),armv7l-timesys-linux-gnueabi-)
	FLOAT_ABI_FLAGS=-mfloat-abi=softfp
endif

ifeq ($(CROSS_COMPILE),armv7l-timesys-linux-gnueabihf-)
	FLOAT_ABI_FLAGS=-mfloat-abi=hard
endif

##############################################################
#
# architecture specific flags
#
##############################################################

ifeq ($(ARCH),arm)
	ARCH_FLAGS=-march=armv7-a -mtune=cortex-a9 -mfpu=neon $(FLOAT_ABI_FLAGS)
endif

##############################################################
#
# build type compiler flags
#
##############################################################

ifeq ($(BUILD_TYPE),debug)
	FFLAGS=-O0 -g $(ARCH_FLAGS)
else
	FFLAGS=-O3 $(ARCH_FLAGS)
endif


