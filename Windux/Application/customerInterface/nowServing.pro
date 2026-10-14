#--------------------------------------------------------------------------------------------------
#
# Note:
# For Asura and Utilite builds you need to disable video playback and chilkat. Do this by entering
# project build settings and adding the following to Build Steps->QMAKE->Additional Arguments:
# CONFIG+=disable_video CONFIG+=disable_chilkat
#
# The above two configuration options will disable video playback and chilkat dependancies.
#
#--------------------------------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += ../shared/logManager
SUBDIRS += ../shared/platform

!disable_video {
    SUBDIRS += ../shared/gstpdCore
    SUBDIRS += ../shared/videoPlayerCore
}

SUBDIRS += ../shared/playlistWidget
SUBDIRS += ../shared/marqueeLabelWidget
SUBDIRS += nowServing
