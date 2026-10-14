
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

SUB_DIRS = dbusinterfaces events eventadapters hydramc hobartsessionman hobartcomms booda test_hydramc test_hobartsessionman test_hobartcomms

all:
	@for dir in $(SUB_DIRS); do cd $$dir; qmake $(QMAKEFLAGS); $(MAKE); cd ..; done


