
#################################################################
#
# File: Makefile
#
# Executes qmake against the Qt ".PRO" file in each sub-directory
# which generates a Makefile.
#
# Then executes make against that Makefile.
#
#################################################################

ifeq ($(BUILD_TYPE),debug)
	QMAKEFLAGS=CONFIG+=release CONFIG+=force_debug_info
else
	QMAKEFLAGS=
endif


SUB_DIRS = gcd nowServing slicer


all:
	@for dir in $(SUB_DIRS); do cd $$dir; qmake $(QMAKEFLAGS) CONFIG+=using_hobart; $(MAKE); cd ..; done

