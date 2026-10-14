TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += ../shared/logManager
SUBDIRS += ../shared/platform
SUBDIRS += ../shared/labelCore
SUBDIRS += ../shared/labelViewerWidget
SUBDIRS += ../shared/marqueeLabelWidget
SUBDIRS += ../shared/marqueeTextEditorWidget
SUBDIRS += ../shared/gstpdCore
SUBDIRS += ../shared/videoPlayerCore
SUBDIRS += ../shared/sessionHandlerUtil
SUBDIRS += ../shared/sessionHandlerWidgets
SUBDIRS += ../shared/sessionHandlerHooks
SUBDIRS += ../shared/Session
SUBDIRS += ../shared/sessionHandler
SUBDIRS += ../shared/playlistWidget

win32 {
    SUBDIRS += ../XpdfWidgetQt/freetype-2.4.11/FreeTypeLib.pro
}

SUBDIRS += ../XpdfWidgetQt/src/XpdfWidgetQt.pro
SUBDIRS += scaleui

