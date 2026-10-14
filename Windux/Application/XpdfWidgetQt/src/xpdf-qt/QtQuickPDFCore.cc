//========================================================================
//
// QtQuickPDFCore.cc
//
// Copyright 2016 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <QCursor>
#include <QDesktopServices>
#include "SplashBitmap.h"
#include "TileMap.h"
#include "XpdfItem.h"
#include "PDFDoc.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "Link.h"
#include "QtQuickPDFCore.h"

//------------------------------------------------------------------------
// QtQuickPDFCore
//------------------------------------------------------------------------

QtQuickPDFCore::QtQuickPDFCore(XpdfItem *itemA,
			       SplashColorPtr paperColor,
			       SplashColorPtr matteColor,
			       GBool reverseVideo):
  PDFCore(splashModeRGB8, 4, reverseVideo, paperColor)
{
  item = itemA;
  hyperlinksEnabled = gTrue;
  pageChangedCbk = NULL;
  midPageChangedCbk = NULL;
  preLoadCbk = NULL;
  postLoadCbk = NULL;
  state->setMatteColor(matteColor);

  oldFirstPage = -1;
  oldMidPage = -1;
}

QtQuickPDFCore::~QtQuickPDFCore() {
}

void QtQuickPDFCore::setBusyCursor(GBool busy) {
  if (busy) {
    doSetCursor(Qt::WaitCursor);
  } else {
    doUnsetCursor();
  }
}

void QtQuickPDFCore::doSetCursor(const QCursor &cursor) {
#ifndef QT_NO_CURSOR
  item->setCursor(cursor);
#endif
}

void QtQuickPDFCore::doUnsetCursor() {
#ifndef QT_NO_CURSOR
  item->unsetCursor();
#endif
}

void QtQuickPDFCore::mouseClick(int wx, int wy) {
  LinkAction *action;
  int pg;
  double xu, yu;

  if (!doc || doc->getNumPages() == 0) {
    return;
  }
  if (hyperlinksEnabled) {
    if (cvtWindowToUser(wx, wy, &pg, &xu, &yu)) {
      if ((action = findLink(pg, xu, yu))) {
	doAction(action);
      }
    }
  }
}

GBool QtQuickPDFCore::doAction(LinkAction *action) {
  LinkActionKind kind;
  LinkDest *dest;
  GString *namedDest;
  char *s;
  GString *fileName;
  GString *actionName;
  Object movieAnnot, obj1, obj2;

  switch (kind = action->getKind()) {

  // GoTo / GoToR action
  case actionGoTo:
  case actionGoToR:
    if (kind == actionGoTo) {
      dest = NULL;
      namedDest = NULL;
      if ((dest = ((LinkGoTo *)action)->getDest())) {
	dest = dest->copy();
      } else if ((namedDest = ((LinkGoTo *)action)->getNamedDest())) {
	namedDest = namedDest->copy();
      }
    } else {
      dest = NULL;
      namedDest = NULL;
      if ((dest = ((LinkGoToR *)action)->getDest())) {
	dest = dest->copy();
      } else if ((namedDest = ((LinkGoToR *)action)->getNamedDest())) {
	namedDest = namedDest->copy();
      }
      s = ((LinkGoToR *)action)->getFileName()->getCString();
      if (isAbsolutePath(s)) {
	fileName = new GString(s);
      } else {
	fileName = appendToPath(grabPath(doc->getFileName()->getCString()), s);
      }
      if (loadFile(fileName) != errNone) {
	if (dest) {
	  delete dest;
	}
	if (namedDest) {
	  delete namedDest;
	}
	delete fileName;
	return gFalse;
      }
      delete fileName;
    }
    if (namedDest) {
      dest = doc->findDest(namedDest);
      delete namedDest;
    }
    if (dest) {
      displayDest(dest);
      delete dest;
    } else {
      if (kind == actionGoToR) {
	displayPage(1, gFalse, gFalse, gTrue);
      }
    }
    break;

  // Launch action
  case actionLaunch:
    fileName = ((LinkLaunch *)action)->getFileName();
    s = fileName->getCString();
    if (fileName->getLength() >= 4 &&
	(!strcmp(s + fileName->getLength() - 4, ".pdf") ||
	 !strcmp(s + fileName->getLength() - 4, ".PDF"))) {
      if (isAbsolutePath(s)) {
	fileName = fileName->copy();
      } else {
	fileName = appendToPath(grabPath(doc->getFileName()->getCString()), s);
      }
      if (loadFile(fileName) != errNone) {
	delete fileName;
	return gFalse;
      }
      delete fileName;
      displayPage(1, gFalse, gFalse, gTrue);
    } else {
#if 0 //~tmp: this could emit a signal
      cmd = fileName->copy();
      if ((params = ((LinkLaunch *)action)->getParams())) {
	cmd->append(' ')->append(params);
      }
      //~ ....
      delete cmd;
#endif
    }
    break;

  // URI action
  case actionURI:
    QDesktopServices::openUrl(QUrl(((LinkURI *)action)->getURI()->getCString(),
				   QUrl::TolerantMode));
    break;

  // Named action
  case actionNamed:
    actionName = ((LinkNamed *)action)->getName();
    if (!actionName->cmp("NextPage")) {
      gotoNextPage(1, gTrue);
    } else if (!actionName->cmp("PrevPage")) {
      gotoPrevPage(1, gTrue, gFalse);
    } else if (!actionName->cmp("FirstPage")) {
      displayPage(1, gTrue, gFalse, gTrue);
    } else if (!actionName->cmp("LastPage")) {
      displayPage(doc->getNumPages(), gTrue, gFalse, gTrue);
    } else if (!actionName->cmp("GoBack")) {
      goBackward();
    } else if (!actionName->cmp("GoForward")) {
      goForward();
    } else if (!actionName->cmp("Quit")) {
      //~tmp: this could emit a signal
    } else {
      error(errSyntaxError, -1,
	    "Unknown named action: '{0:t}'", actionName);
      return gFalse;
    }
    break;

  // Movie action
  case actionMovie:
    //~tmp: this could emit a signal
    break;

  // unimplemented actions
  case actionJavaScript:
  case actionSubmitForm:
  case actionHide:
    return gFalse;

  // unknown action type
  case actionUnknown:
    error(errSyntaxError, -1, "Unknown link action type: '{0:t}'",
	  ((LinkUnknown *)action)->getAction());
    return gFalse;
  }

  return gTrue;
}

int QtQuickPDFCore::loadFile(GString *fileName, GString *ownerPassword,
			     GString *userPassword) {
  int err;

  err = PDFCore::loadFile(fileName, ownerPassword, userPassword);
  if (err == errNone) {
    oldFirstPage = oldMidPage = -1;
  }
  return err;
}

#ifdef _WIN32
int QtQuickPDFCore::loadFile(wchar_t *fileName, int fileNameLen,
			     GString *ownerPassword,
			     GString *userPassword) {
  int err;

  err = PDFCore::loadFile(fileName, fileNameLen, ownerPassword, userPassword);
  if (err == errNone) {
    oldFirstPage = oldMidPage = -1;
  }
  return err;
}
#endif

void QtQuickPDFCore::finishUpdate(GBool addToHist, GBool checkForChangedFile) {
  int firstPage, midPage;

  PDFCore::finishUpdate(addToHist, checkForChangedFile);
  firstPage = getPageNum();
  if (doc && firstPage != oldFirstPage && pageChangedCbk) {
    (*pageChangedCbk)(pageChangedCbkData, firstPage);
  }
  oldFirstPage = firstPage;
  midPage = getMidPageNum();
  if (doc && midPage != oldMidPage && midPageChangedCbk) {
    (*midPageChangedCbk)(midPageChangedCbkData, midPage);
  }
  oldMidPage = midPage;
}

QImage QtQuickPDFCore::getImage() {
  SplashBitmap *bitmap;

  bitmap = getWindowBitmap(gTrue);
  QImage image(bitmap->getDataPtr(), bitmap->getWidth(),
	       bitmap->getHeight(), QImage::Format_RGB888);
  return image;
}

void QtQuickPDFCore::resize(int w, int h) {
  setWindowSize(w, h);
}

void QtQuickPDFCore::tick() {
  PDFCore::tick();
}

void QtQuickPDFCore::invalidate(int x, int y, int w, int h) {
  item->update();
}

void QtQuickPDFCore::updateScrollbars() {
  int horizLimit, vertLimit;

  tileMap->getScrollLimits(&horizLimit, &vertLimit);
  item->updateScrollParams(horizLimit, vertLimit,
			   state->getScrollX(), state->getScrollY());
}

void QtQuickPDFCore::preLoad() {
  if (preLoadCbk) {
    (*preLoadCbk)(preLoadCbkData);
  }
}

void QtQuickPDFCore::postLoad() {
  if (postLoadCbk) {
    (*postLoadCbk)(postLoadCbkData);
  }
}
