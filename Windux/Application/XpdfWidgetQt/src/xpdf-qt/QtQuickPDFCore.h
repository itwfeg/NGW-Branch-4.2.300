//========================================================================
//
// QtQuickPDFCore.h
//
// Copyright 2016 Glyph & Cog, LLC
//
//========================================================================

#ifndef QTQUICKPDFCORE_H
#define QTQUICKPDFCORE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <QImage>
#include "PDFCore.h"

class XpdfItem;

//------------------------------------------------------------------------
// callbacks
//------------------------------------------------------------------------

typedef void (*QtPDFPageChangedCbk)(void *data, int pageNum);
typedef void (*QtPDFLoadCbk)(void *data);

//------------------------------------------------------------------------
// QtQuickPDFCore
//------------------------------------------------------------------------

class QtQuickPDFCore: public PDFCore {
public:

  QtQuickPDFCore(XpdfItem *itemA,
		 SplashColorPtr paperColor, SplashColorPtr matteColor,
		 GBool reverseVideo);
  virtual ~QtQuickPDFCore();

  // Load a new file.  Returns pdfOk or error code.
  virtual int loadFile(GString *fileName, GString *ownerPassword = NULL,
		       GString *userPassword = NULL);

#ifdef _WIN32
  // Load a new file.  Returns pdfOk or error code.
  virtual int loadFile(wchar_t *fileName, int fileNameLen,
		       GString *ownerPassword = NULL,
		       GString *userPassword = NULL);
#endif

  // Called after any update is complete.
  virtual void finishUpdate(GBool addToHist, GBool checkForChangedFile);

  QImage getImage();

  void resize(int w, int h);

  void tick();

  virtual void setBusyCursor(GBool busy);
  void doSetCursor(const QCursor &cursor);
  void doUnsetCursor();

  void mouseClick(int wx, int wy);

  void enableHyperlinks(GBool on) { hyperlinksEnabled = on; }
  GBool getHyperlinksEnabled() { return hyperlinksEnabled; }
  GBool doAction(LinkAction *action);

  void setPageChangedCbk(QtPDFPageChangedCbk cbk, void *data)
    { pageChangedCbk = cbk; pageChangedCbkData = data; }
  void setMidPageChangedCbk(QtPDFPageChangedCbk cbk, void *data)
    { midPageChangedCbk = cbk; midPageChangedCbkData = data; }
  void setPreLoadCbk(QtPDFLoadCbk cbk, void *data)
    { preLoadCbk = cbk; preLoadCbkData = data; }
  void setPostLoadCbk(QtPDFLoadCbk cbk, void *data)
    { postLoadCbk = cbk; postLoadCbkData = data; }

protected:

  virtual void invalidate(int x, int y, int w, int h);

  virtual void updateScrollbars();

private:

  virtual void preLoad();
  virtual void postLoad();

  XpdfItem *item;

  GBool hyperlinksEnabled;

  int oldFirstPage;
  int oldMidPage;

  QtPDFPageChangedCbk pageChangedCbk;
  void *pageChangedCbkData;
  QtPDFPageChangedCbk midPageChangedCbk;
  void *midPageChangedCbkData;
  QtPDFLoadCbk preLoadCbk;
  void *preLoadCbkData;
  QtPDFLoadCbk postLoadCbk;
  void *postLoadCbkData;
};

#endif // QTQUICKPDFCORE_H
