
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


SUB_DIRS = \
	logManager \
	platform \
	comm \
	labelCore \
	labelViewerWidget \
	marqueeLabelWidget \
	marqueeTextEditorWidget \
	gstpdCore \
	videoPlayerCore \
	playlistWidget \
	sessionHandlerUtil \
	sessionHandlerWidgets \
	sessionHandlerHooks \
	Session \
	sessionHandler

all:
	@for dir in $(SUB_DIRS); do cd $$dir; qmake $(QMAKEFLAGS); $(MAKE); cd ..; done
	cd gstpd; qmake $(QMAKEFLAGS); $(MAKE); cd ..
