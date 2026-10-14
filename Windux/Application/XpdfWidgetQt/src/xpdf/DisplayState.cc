//========================================================================
//
// DisplayState.cc
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "TileMap.h"
#include "TileCache.h"
#include "TileCompositor.h"
#include "DisplayState.h"

#ifdef HIGHLIGHTED_REGIONS

//------------------------------------------------------------------------
// PDFRegion
//------------------------------------------------------------------------

PDFRegion::PDFRegion() {
  label = NULL;
}

PDFRegion::~PDFRegion() {
  if (label) {
    delete label;
  }
}

#endif // HIGHLIGHTED_REGIONS

//------------------------------------------------------------------------
// DisplayState
//------------------------------------------------------------------------

DisplayState::DisplayState(int maxTileWidthA, int maxTileHeightA,
			   int tileCacheSizeA, int nWorkerThreadsA,
			   SplashColorMode colorModeA, int bitmapRowPadA) {
  int i;

  maxTileWidth = maxTileWidthA;
  maxTileHeight = maxTileHeightA;
  tileCacheSize = tileCacheSizeA;
  nWorkerThreads = nWorkerThreadsA;
  colorMode = colorModeA;
  bitmapRowPad = bitmapRowPadA;

  tileMap = NULL;
  tileCache = NULL;
  tileCompositor = NULL;

  for (i = 0; i < splashColorModeNComps[colorMode]; ++i) {
    paperColor[i] = 0xff;
    matteColor[i] = 0x80;
  }
  if (colorMode == splashModeRGB8 || colorMode == splashModeBGR8) {
    selectColor[0] = 0x80;
    selectColor[1] = 0x80;
    selectColor[2] = 0xff;
  } else {
    for (i = 0; i < splashColorModeNComps[colorMode]; ++i) {
      selectColor[i] = 0xa0;
    }
  }
  reverseVideo = gFalse;

  doc = NULL;

  winW = winH = 100;
  displayMode = displayContinuous;
  zoom = 100;
  rotate = 0;
  scrollPage = 0;
  scrollX = scrollY = 0;

  selectRects = NULL;

#ifdef HIGHLIGHTED_REGIONS
  regions = new GList();
  selectedRegion = NULL;
#endif

}

DisplayState::~DisplayState() {
  if (selectRects) {
    deleteGList(selectRects, SelectRect);
  }
#ifdef HIGHLIGHTED_REGIONS
  deleteGList(regions, PDFRegion);
#endif
}

void DisplayState::setPaperColor(SplashColorPtr paperColorA) {
  splashColorCopy(paperColor, paperColorA);
  tileCache->paperColorChanged();
  tileCompositor->paperColorChanged();
}

void DisplayState::setMatteColor(SplashColorPtr matteColorA) {
  splashColorCopy(matteColor, matteColorA);
  tileCompositor->matteColorChanged();
}

void DisplayState::setSelectColor(SplashColorPtr selectColorA) {
  splashColorCopy(selectColor, selectColorA);
  tileCompositor->selectColorChanged();
}

void DisplayState::setReverseVideo(GBool reverseVideoA) {
  if (reverseVideo != reverseVideoA) {
    reverseVideo = reverseVideoA;
    tileCache->reverseVideoChanged();
    tileCompositor->reverseVideoChanged();
  }
}

void DisplayState::setDoc(PDFDoc *docA) {
  doc = docA;
  tileMap->docChanged();
  tileCache->docChanged();
  tileCompositor->docChanged();
}

void DisplayState::setWindowSize(int winWA, int winHA) {
  if (winW != winWA || winH != winHA) {
    winW = winWA;
    winH = winHA;
    tileMap->windowSizeChanged();
    tileCompositor->windowSizeChanged();
  }
}

void DisplayState::setDisplayMode(DisplayMode displayModeA) {
  if (displayMode != displayModeA) {
    displayMode = displayModeA;
    tileMap->displayModeChanged();
    tileCompositor->displayModeChanged();
  }
}

void DisplayState::setZoom(double zoomA) {
  if (zoom != zoomA) {
    zoom = zoomA;
    tileMap->zoomChanged();
    tileCompositor->zoomChanged();
  }
}

void DisplayState::setRotate(int rotateA) {
  if (rotate != rotateA) {
    rotate = rotateA;
    tileMap->rotateChanged();
    tileCompositor->rotateChanged();
  }
}

void DisplayState::setScrollPosition(int scrollPageA,
				     int scrollXA, int scrollYA) {
  if (scrollPage != scrollPageA ||
      scrollX != scrollXA ||
      scrollY != scrollYA) {
    scrollPage = scrollPageA;
    scrollX = scrollXA;
    scrollY = scrollYA;
    tileMap->scrollPositionChanged();
    tileCompositor->scrollPositionChanged();
  }
}

void DisplayState::setSelection(int selectPage,
				double selectX0, double selectY0,
				double selectX1, double selectY1) {
  GList *rects;
  SelectRect *rect;

  rect = new SelectRect(selectPage, selectX0, selectY0, selectX1, selectY1);
  rects = new GList();
  rects->append(rect);
  setSelection(rects);
}

void DisplayState::setSelection(GList *selectRectsA) {
  SelectRect *rect, *rectA;
  int i;

  if (!selectRects && !selectRectsA) {
    return;
  }
  if (selectRects && selectRectsA &&
      selectRects->getLength() == selectRectsA->getLength()) {
    for (i = 0; i < selectRects->getLength(); ++i) {
      rect = (SelectRect *)selectRects->get(i);
      rectA = (SelectRect *)selectRectsA->get(i);
      if (*rect != *rectA) {
	break;
      }
    }
    if (i == selectRects->getLength()) {
      deleteGList(selectRectsA, SelectRect);
      return;
    }
  }
  if (selectRects) {
    deleteGList(selectRects, SelectRect);
  }
  selectRects = selectRectsA;
  tileCompositor->selectionChanged();
}

void DisplayState::clearSelection() {
  setSelection(NULL);
}

int DisplayState::getNumSelectRects() {
  if (!selectRects) {
    return 0;
  }
  return selectRects->getLength();
}

SelectRect *DisplayState::getSelectRect(int idx) {
  return (SelectRect *)selectRects->get(idx);
}

void DisplayState::optionalContentChanged() {
  tileCache->optionalContentChanged();
  tileCompositor->optionalContentChanged();
}

#ifdef HIGHLIGHTED_REGIONS

PDFRegion *DisplayState::addRegion(int pg, double xMin, double yMin,
				   double xMax, double yMax,
				   SplashColor color,
				   SplashColorPtr selColor,
				   GBool selectable, GBool resizable,
				   char *label) {
  PDFRegion *region;

  region = new PDFRegion();
  region->page = pg;
  region->xMin = xMin < xMax ? xMin : xMax;
  region->xMax = xMin < xMax ? xMax : xMin;
  region->yMin = yMin < yMax ? yMin : yMax;
  region->yMax = yMin < yMax ? yMax : yMin;
  splashColorCopy(region->color, color);
  splashColorCopy(region->selectColor, selColor);
  region->selectable = selectable;
  region->resizable = resizable && selectable;
  if (label) {
    region->label = new GString(label);
  } else {
    region->label = NULL;
  }
  regions->append(region);
  tileCompositor->regionsChanged();
  return region;
}

void DisplayState::resizeRegion(PDFRegion *region, double xMin, double yMin,
				double xMax, double yMax) {
  region->xMin = xMin;
  region->yMin = yMin;
  region->xMax = xMax;
  region->yMax = yMax;
  tileCompositor->regionsChanged();
}

void DisplayState::updateRegion(PDFRegion *region,
				SplashColorPtr color,
				SplashColorPtr selectColor,
				GBool selectable, GBool resizable,
				char *label) {
  splashColorCopy(region->color, color);
  splashColorCopy(region->selectColor, selectColor);
  region->selectable = selectable;
  region->resizable = resizable && selectable;
  if (region->label && label == region->label->getCString()) {
    // no change
  } else {
    if (region->label) {
      delete region->label;
    }
    if (label) {
      region->label = new GString(label);
    } else {
      region->label = NULL;
    }
  }
  tileCompositor->regionsChanged();
}

void DisplayState::deleteRegion(PDFRegion *region) {
  int i;

  for (i = 0; i < regions->getLength(); ++i) {
    if ((PDFRegion *)regions->get(i) == region) {
      break;
    }
  }
  if (i == regions->getLength()) {
    return;
  }
  regions->del(i);
  delete region;
  tileCompositor->regionsChanged();
}

void DisplayState::deleteAllRegions() {
  deleteGList(regions, PDFRegion);
  regions = new GList();
  tileCompositor->regionsChanged();
}

void DisplayState::setSelectedRegion(PDFRegion *region) {
  if (selectedRegion != region) {
    selectedRegion = region;
    tileCompositor->regionsChanged();
  }
}

void DisplayState::clearSelectedRegion() {
  if (selectedRegion) {
    selectedRegion = NULL;
    tileCompositor->regionsChanged();
  }
}

int DisplayState::getNumRegions() {
  return regions->getLength();
}

PDFRegion *DisplayState::getRegion(int idx) {
  if (idx < 0 || idx >= regions->getLength()) {
    return NULL;
  }
  return (PDFRegion *)regions->get(idx);
}

PDFRegion *DisplayState::findRegion(int pg, int x, int y,
				    int *resizeX, int *resizeY) {
  PDFRegion *region;
  int x0, y0, x1, y1, x2, y2, t, i;

  for (i = 0; i < regions->getLength(); ++i) {
    region = (PDFRegion *)regions->get(i);
    if (region->page == pg) {
      tileMap->cvtUserToDev(region->page, region->xMin, region->yMin, &x0, &y0);
      tileMap->cvtUserToDev(region->page, region->xMax, region->yMax, &x1, &y1);
      if (x0 > x1) {
	t = x0;  x0 = x1;  x1 = t;
      }
      if (y0 > y1) {
	t = y0;  y0 = y1;  y1 = t;
      }
      if (region->resizable) {
	x2 = (x0 + x1) / 2;
	y2 = (y0 + y1) / 2;
	if (x0 - regionHandleSize/2 <= x &&
	    x < x0 + regionHandleSize/2) {
	  if (y0 - regionHandleSize/2 <= y &&
	      y < y0 + regionHandleSize/2) {
	    *resizeX = -1;
	    *resizeY = -1;
	    return region;
	  } else if (y1 - regionHandleSize/2 <= y &&
		     y < y1 + regionHandleSize/2) {
	    *resizeX = -1;
	    *resizeY = 1;
	    return region;
	  } else if (y2 - regionHandleSize/2 <= y &&
		     y < y2 + regionHandleSize/2) {
	    *resizeX = -1;
	    *resizeY = 0;
	    return region;
	  }
	} else if (x1 - regionHandleSize/2 <= x &&
		   x < x1 + regionHandleSize/2) {
	  if (y0 - regionHandleSize/2 <= y &&
	      y < y0 + regionHandleSize/2) {
	    *resizeX = 1;
	    *resizeY = -1;
	    return region;
	  } else if (y1 - regionHandleSize/2 <= y &&
		     y < y1 + regionHandleSize/2) {
	    *resizeX = 1;
	    *resizeY = 1;
	    return region;
	  } else if (y2 - regionHandleSize/2 <= y &&
		     y < y2 + regionHandleSize/2) {
	    *resizeX = 1;
	    *resizeY = 0;
	    return region;
	  }
	} else if (x2 - regionHandleSize/2 <= x &&
		   x < x2 + regionHandleSize/2) {
	  if (y0 - regionHandleSize/2 <= y &&
	      y < y0 + regionHandleSize/2) {
	    *resizeX = 0;
	    *resizeY = -1;
	    return region;
	  } else if (y1 - regionHandleSize/2 <= y &&
		     y < y1 + regionHandleSize/2) {
	    *resizeX = 0;
	    *resizeY = 1;
	    return region;
	  }
	}
      }
      if (x0 <= x && x < x1 && y0 <= y && y < y1) {
	*resizeX = 0;
	*resizeY = 0;
	return region;
      }
    }
  }
  return NULL;
}

#endif


void DisplayState::forceRedraw() {
  tileMap->forceRedraw();
  tileCache->forceRedraw();
  tileCompositor->forceRedraw();
}
