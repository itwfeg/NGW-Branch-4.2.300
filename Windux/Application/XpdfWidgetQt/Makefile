
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


SUB_DIRS = src



all:
	@for dir in $(SUB_DIRS); do cd $$dir; qmake; $(MAKE); cd ..; done


