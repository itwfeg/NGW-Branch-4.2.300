//========================================================================
//
// TileCompositor.cc
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <math.h>
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "SplashBitmap.h"
#ifdef HIGHLIGHTED_REGIONS
#include "FoFiTrueType.h"
#include "Splash.h"
#include "SplashPattern.h"
#include "SplashFontEngine.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"
#include "BuiltinFont.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "GfxFont.h"
#endif // HIGHLIGHTED_REGIONS
#include "GlobalParams.h"
#include "DisplayState.h"
#include "TileMap.h"
#include "TileCache.h"
#include "TileCompositor.h"

//------------------------------------------------------------------------

// Divide a 16-bit value (in [0, 255*255]) by 255, returning an 8-bit result.
static inline Guchar div255(int x) {
  return (Guchar)((x + (x >> 8) + 0x80) >> 8);
}

//------------------------------------------------------------------------

TileCompositor::TileCompositor(DisplayState *stateA,
			       TileMap *tileMapA,
			       TileCache *tileCacheA) {
  state = stateA;
  state->setTileCompositor(this);
  tileMap = tileMapA;
  tileCache = tileCacheA;
  bitmap = NULL;
  bitmapValid = gFalse;
#ifdef HIGHLIGHTED_REGIONS
  fontEngine = NULL;
#endif
}

TileCompositor::~TileCompositor() {
  delete bitmap;
#ifdef HIGHLIGHTED_REGIONS
  if (fontEngine) {
    delete fontEngine;
  }
#endif
}

SplashBitmap *TileCompositor::getBitmap(GBool *finished) {
  GList *tiles;
  PlacedTileDesc *tile;
  SplashBitmap *tileBitmap;
  GBool allTilesFinished, tileFinished;
  int xSrc, ySrc, xDest, yDest, w, h, i;

  if (bitmapValid) {
    *finished = gTrue;
    return bitmap;
  }
  if (!bitmap ||
      bitmap->getWidth() != state->getWinW() ||
      bitmap->getHeight() != state->getWinH()) {
    if (bitmap) {
      delete bitmap;
    }
    bitmap = new SplashBitmap(state->getWinW(), state->getWinH(),
			      state->getBitmapRowPad(), state->getColorMode(),
			      gFalse);
  }
  clearBitmap();

  //--- PDF content
  allTilesFinished = gTrue;
  tiles = tileMap->getTileList();
  tileCache->setActiveTileList(tiles);
  for (i = 0; i < tiles->getLength(); ++i) {
    tile = (PlacedTileDesc *)tiles->get(i);
    if (tile->px >= 0) {
      xSrc = 0;
      xDest = tile->px;
      w = tile->tw;
    } else {
      xSrc = -tile->px;
      xDest = 0;
      w = tile->tw + tile->px;
    }
    if (xDest + w > state->getWinW()) {
      w = state->getWinW() - xDest;
    }
    if (tile->py >= 0) {
      ySrc = 0;
      yDest = tile->py;
      h = tile->th;
    } else {
      ySrc = -tile->py;
      yDest = 0;
      h = tile->th + tile->py;
    }
    if (yDest + h > state->getWinH()) {
      h = state->getWinH() - yDest;
    }
    if (w > 0 && h > 0) {
      if ((tileBitmap = tileCache->getTileBitmap(tile, &tileFinished))) {
	blit(tileBitmap, xSrc, ySrc,
	     bitmap, xDest, yDest, w, h, !tileFinished);
      } else {
	fill(xDest, yDest, w, h, state->getPaperColor());
      }
      allTilesFinished &= tileFinished;
    }
  }

#ifdef HIGHLIGHTED_REGIONS
  //--- highlighted regions
  for (i = 0; i < state->getNumRegions(); ++i) {
    drawHighlightedRegion(state->getRegion(i));
  }
#endif

  //--- selection
  if (state->hasSelection()) {
    drawSelection();
  }

  if (finished) {
    *finished = allTilesFinished;
  }
  bitmapValid = allTilesFinished;

  return bitmap;
}

void TileCompositor::drawSelection() {
  SelectRect *rect;
  int x0, y0, x1, y1, t, i;

  for (i = 0; i < state->getNumSelectRects(); ++i) {
    rect = state->getSelectRect(i);
    tileMap->cvtUserToWindow(rect->page, rect->x0, rect->y0, &x0, &y0);
    tileMap->cvtUserToWindow(rect->page, rect->x1, rect->y1, &x1, &y1);
    if (x0 > x1) {
      t = x0;  x0 = x1;  x1 = t;
    }
    if (y0 > y1) {
      t = y0;  y0 = y1;  y1 = t;
    }
    if (x0 < 0) {
      x0 = 0;
    } else if (x0 > state->getWinW()) {
      x0 = state->getWinW();
    }
    if (y0 < 0) {
      y0 = 0;
    } else if (y0 > state->getWinH()) {
      y0 = state->getWinH();
    }
    if (x1 < 0) {
      x1 = 0;
    } else if (x1 > state->getWinW()) {
      x1 = state->getWinW();
    }
    if (y1 < 0) {
      y1 = 0;
    } else if (y1 > state->getWinH()) {
      y1 = state->getWinH();
    }
    // NB: in any of the non-continuous modes, if selectPage isn't
    // displayed, cvtUserToWindow will return 0,0 -- so we need to
    // check for x0 < x1 and y0 < y1 here
    if (x0 < x1 && y0 < y1) {
      applySelection(x0, y0, x1 - x0, y1 - y0, state->getSelectColor());
    }
  }
}

#ifdef HIGHLIGHTED_REGIONS
void TileCompositor::drawHighlightedRegion(PDFRegion *region) {
  SplashColorPtr color;
  SplashColor handleColor, textColor, bgColor;
  Splash *splash;
  SplashCoord mat[6], textMat[4];
  SplashCoord x, y, w;
  BuiltinFont *bf;
  SplashFont *font;
  Gushort cw;
  int builtinFontIdx, x0, y0, x1, y1, x2, y2, t, c, i, j;

  //--- get the rectangle
  tileMap->cvtUserToWindow(region->page, region->xMin, region->yMin,
			   &x0, &y0);
  tileMap->cvtUserToWindow(region->page, region->xMax, region->yMax,
			   &x1, &y1);
  if (x0 > x1) {
    t = x0;  x0 = x1;  x1 = t;
  }
  if (y0 > y1) {
    t = y0;  y0 = y1;  y1 = t;
  }
  if (x0 >= state->getWinW() || x1 < 0 ||
      y0 >= state->getWinH() || y1 < 0) {
    return;
  }

  //--- draw the colored rectangle
  if (region == state->getSelectedRegion()) {
    color = region->selectColor;
  } else {
    color = region->color;
  }
  applySelection(x0, y0, x1 - x0, y1 - y0, color);

  //--- draw the resize handles
  if (region == state->getSelectedRegion() &&
      region->resizable) {
    handleColor[0] = handleColor[1] = handleColor[2] = 0x00;
    fill(x0 - regionHandleSize/2, y0 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x0 - regionHandleSize/2, y1 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x1 - regionHandleSize/2, y0 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x1 - regionHandleSize/2, y1 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    x2 = (x0 + x1) / 2;
    y2 = (y0 + y1) / 2;
    fill(x0 - regionHandleSize/2, y2 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x1 - regionHandleSize/2, y2 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x2 - regionHandleSize/2, y0 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
    fill(x2 - regionHandleSize/2, y1 - regionHandleSize/2,
	 regionHandleSize, regionHandleSize, handleColor);
  }

  //--- draw the label
  if (region->label &&
      y1 - y0 >= regionLabelSize + 6) {
    //~ could cache the Splash object
    splash = new Splash(bitmap, gTrue, NULL);
    mat[0] = 1;  mat[1] = 0;
    mat[2] = 0;  mat[3] = 1;
    mat[4] = 0;  mat[5] = 0;
    textMat[0] = regionLabelSize;  textMat[1] = 0;
    textMat[2] = 0;                textMat[3] = -regionLabelSize;
    builtinFontIdx = 4; // Helvetica
    splash->setMatrix(mat);
    font = getFont(builtinFontIdx, textMat, mat);
    if (font) {
      bf = builtinFontSubst[builtinFontIdx];
      textColor[0] = textColor[1] = textColor[2] = 0x00;
      splash->setFillPattern(new SplashSolidColor(textColor));
      x = x1 - 3;
      for (i = region->label->getLength() - 1; i >= 0; --i) {
	c = region->label->getChar(i) & 0xff;
	if (winAnsiEncoding[c]) {
	  bf->widths->getWidth(winAnsiEncoding[c], &cw);
	  w = regionLabelSize * cw * 0.001;
	  if (x - w < x0 + 3) {
	    break;
	  }
	  x -= w;
	}
      }
      bgColor[0] = bgColor[1] = bgColor[2] = 0xff;
      x2 = (int)x;
      fill(x2, y0 + 3, x1 - 3 - x2, regionLabelSize, bgColor);
      x = x1 - 3;
      y = (SplashCoord)(y0 + 3) + (SplashCoord)(0.75 * regionLabelSize);
      for (j = region->label->getLength() - 1; j > i; --j) {
	c = region->label->getChar(j) & 0xff;
	if (winAnsiEncoding[c]) {
	  bf->widths->getWidth(winAnsiEncoding[c], &cw);
	  w = regionLabelSize * cw * 0.001;
	  x -= w;
	  splash->fillChar(x, y, c, font);
	}
      }
    }
    delete splash;
  }
}
#endif // HIGHLIGHTED_REGIONS

void TileCompositor::paperColorChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::matteColorChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::selectColorChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::reverseVideoChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::docChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::windowSizeChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::displayModeChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::zoomChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::rotateChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::scrollPositionChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::selectionChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::regionsChanged() {
  bitmapValid = gFalse;
}

void TileCompositor::optionalContentChanged() {
  bitmapValid = gFalse;
}


void TileCompositor::forceRedraw() {
  bitmapValid = gFalse;
}

// Clear the bitmap to matteColor.  This only supports the RGB8 and
// BGR8 color modes.
void TileCompositor::clearBitmap() {
  SplashColorPtr data, row, p;
  SplashBitmapRowSize rowSize;
  int w, h, x, y;

  w = bitmap->getWidth();
  h = bitmap->getHeight();
  rowSize = bitmap->getRowSize();
  data = bitmap->getDataPtr();

  switch (state->getColorMode()) {
  case splashModeRGB8:
    if (state->getMatteColor()[0] == state->getMatteColor()[1] &&
	state->getMatteColor()[1] == state->getMatteColor()[2]) {
      if (rowSize < 0) {
	memset(data + rowSize * (h - 1), state->getMatteColor()[0],
	       -rowSize * h);
      } else {
	memset(data, state->getMatteColor()[0], rowSize * h);
      }
    } else {
      row = data;
      for (y = 0; y < h; ++y) {
	p = row;
	for (x = 0; x < w; ++x) {
	  *p++ = state->getMatteColor()[0];
	  *p++ = state->getMatteColor()[1];
	  *p++ = state->getMatteColor()[2];
	}
	row += rowSize;
      }
    }
    break;
  case splashModeBGR8:
    if (state->getMatteColor()[0] == state->getMatteColor()[1] &&
	state->getMatteColor()[1] == state->getMatteColor()[2]) {
      if (rowSize < 0) {
	memset(data + rowSize * (h - 1), state->getMatteColor()[0],
	       -rowSize * h);
      } else {
	memset(data, state->getMatteColor()[0], rowSize * h);
      }
    } else {
      row = data;
      for (y = 0; y < h; ++y) {
	p = row;
	for (x = 0; x < w; ++x) {
	  *p++ = state->getMatteColor()[2];
	  *p++ = state->getMatteColor()[1];
	  *p++ = state->getMatteColor()[0];
	}
	row += rowSize;
      }
    }
    break;
  default:
    break;
  }
}

// Copy [xSrc, ySrc, w, h] from srcBitmap to [xDest, yDest] in
// destBitmap.  This only supports the RGB8 and BGR8 color modes.
// If [compositeWithPaper] is true, composites the source bitmap
// with the paper color (used for incremental updates).
void TileCompositor::blit(SplashBitmap *srcBitmap, int xSrc, int ySrc,
			  SplashBitmap *destBitmap, int xDest, int yDest,
			  int w, int h, GBool compositeWithPaper) {
  SplashColorPtr srcData, destData, srcP, destP;
  SplashColorPtr paperColor;
  Guchar *alphaData, *alphaP;
  Guchar alpha, alpha1;
  SplashBitmapRowSize srcRowSize, destRowSize;
  size_t alphaRowSize;
  int x, y;

  srcData = srcBitmap->getDataPtr();
  srcRowSize = srcBitmap->getRowSize();
  destData = destBitmap->getDataPtr();
  destRowSize = destBitmap->getRowSize();

  if (compositeWithPaper && (alphaData = srcBitmap->getAlphaPtr())) {
    alphaRowSize = srcBitmap->getAlphaRowSize();
    paperColor = state->getPaperColor();
    for (y = 0; y < h; ++y) {
      destP = &destData[(yDest + y) * destRowSize + 3 * xDest];
      srcP = &srcData[(ySrc + y) * srcRowSize + 3 * xSrc];
      alphaP = &alphaData[(ySrc + y) * alphaRowSize + xSrc];
      for (x = 0; x < w; ++x) {
	alpha = *alphaP++;
	//~ this needs to swap paperColor 0/1/2 in BGR8 mode (?)
	if (alpha == 255) {
	  destP[0] = srcP[0];
	  destP[1] = srcP[1];
	  destP[2] = srcP[2];
	} else if (alpha == 0) {
	  destP[0] = paperColor[0];
	  destP[1] = paperColor[1];
	  destP[2] = paperColor[2];
	} else {
	  alpha1 = (Guchar)(255 - alpha);
	  destP[0] = div255(alpha1 * paperColor[0] + alpha * srcP[0]);
	  destP[1] = div255(alpha1 * paperColor[1] + alpha * srcP[1]);
	  destP[2] = div255(alpha1 * paperColor[2] + alpha * srcP[2]);
	}
	srcP += 3;
	destP += 3;
      }
    }
  } else {
    for (y = 0; y < h; ++y) {
      destP = &destData[(yDest + y) * destRowSize + 3 * xDest];
      srcP = &srcData[(ySrc + y) * srcRowSize + 3 * xSrc];
      memcpy(destP, srcP, 3 * w);
    }
  }
}

// Fill [xDest, yDest, w, h] with color.  This only supports the RGB8
// and BGR8 color modes.
void TileCompositor::fill(int xDest, int yDest, int w, int h,
			  SplashColorPtr color) {
  SplashColorPtr data, p;
  Guchar c0, c1, c2;
  SplashBitmapRowSize rowSize;
  int x, y;

  if (xDest < 0) {
    w += xDest;
    xDest = 0;
  }
  if (xDest + w > state->getWinW()) {
    w = state->getWinW() - xDest;
  }
  if (w <= 0) {
    return;
  }
  if (yDest < 0) {
    h += yDest;
    yDest = 0;
  }
  if (yDest + h > state->getWinH()) {
    h = state->getWinH() - yDest;
  }
  if (h <= 0) {
    return;
  }
  data = bitmap->getDataPtr();
  rowSize = bitmap->getRowSize();
  if (bitmap->getMode() == splashModeRGB8) {
    c0 = color[0];
    c1 = color[1];
    c2 = color[2];
  } else {
    c0 = color[2];
    c1 = color[1];
    c2 = color[0];
  }
  for (y = 0; y < h; ++y) {
    p = &data[(yDest + y) * rowSize + 3 * xDest];
    for (x = 0; x < w; ++x) {
      p[0] = c0;
      p[1] = c1;
      p[2] = c2;
      p += 3;
    }
  }
}

// Apply a selection of <color> to the specified rectangle.  This only
// supports the RGB8 and BGR8 color modes.
void TileCompositor::applySelection(int xDest, int yDest, int w, int h,
				    SplashColorPtr color) {
  SplashColorPtr data, p;
  Guchar c0, c1, c2;
  SplashBitmapRowSize rowSize;
  int x, y;

  if (xDest < 0) {
    w += xDest;
    xDest = 0;
  }
  if (xDest + w > state->getWinW()) {
    w = state->getWinW() - xDest;
  }
  if (w <= 0) {
    return;
  }
  if (yDest < 0) {
    h += yDest;
    yDest = 0;
  }
  if (yDest + h > state->getWinH()) {
    h = state->getWinH() - yDest;
  }
  if (h <= 0) {
    return;
  }
  data = bitmap->getDataPtr();
  rowSize = bitmap->getRowSize();
  if (bitmap->getMode() == splashModeRGB8) {
    c0 = color[0];
    c1 = color[1];
    c2 = color[2];
  } else {
    c0 = color[2];
    c1 = color[1];
    c2 = color[0];
  }
  for (y = 0; y < h; ++y) {
    p = &data[(yDest + y) * rowSize + 3 * xDest];
    for (x = 0; x < w; ++x) {
      p[0] = (Guchar)((3 * p[0] + c0) / 4);
      p[1] = (Guchar)((3 * p[1] + c1) / 4);
      p[2] = (Guchar)((3 * p[2] + c2) / 4);
      p += 3;
    }
  }
}

//------------------------------------------------------------------------

#ifdef HIGHLIGHTED_REGIONS

class TileCompositorFontFileID: public SplashFontFileID {
public:

  TileCompositorFontFileID(int builtinFontIdxA) {
    builtinFontIdx = builtinFontIdxA;
    oblique = 0;
  }

  ~TileCompositorFontFileID() {}

  GBool matches(SplashFontFileID *id) {
    return ((TileCompositorFontFileID *)id)->builtinFontIdx == builtinFontIdx;
  }

  void setOblique(SplashCoord obliqueA) { oblique = obliqueA; }
  SplashCoord getOblique() { return oblique; }

private:

  int builtinFontIdx;
  SplashCoord oblique;
};

SplashFont *TileCompositor::getFont(int builtinFontIdx, SplashCoord *mat,
				    SplashCoord *globalMat) {
  TileCompositorFontFileID *id;
  SplashFontFile *fontFile;
  GString *fontName;
  GfxFontLoc *fontLoc;
#if LOAD_FONTS_FROM_MEM
  GString *fontBuf;
  FILE *extFontFile;
  char blk[4096];
  int n;
#endif
  FoFiTrueType *ff;
  int *codeToGID;
  SplashCoord oblique;
  SplashCoord textMat[4];
  SplashFont *font;
  Unicode u;
  int cmap, cmapPlatform, cmapEncoding, i;

  // create the font engine
  if (!fontEngine) {
    fontEngine = new SplashFontEngine(
#if HAVE_FREETYPE_H
				      globalParams->getEnableFreeType(),
				      globalParams->getDisableFreeTypeHinting()
				        ? splashFTNoHinting : 0,
#endif
				      globalParams->getAntialias());
  }

  // check the font file cache
  id = new TileCompositorFontFileID(builtinFontIdx);
  if ((fontFile = fontEngine->getFontFile(id))) {
    delete id;

  // load the font file
  } else {
    fontName = new GString(builtinFontSubst[builtinFontIdx]->name);
    fontLoc = GfxFont::locateBase14Font(fontName);
    delete fontName;
    if (!fontLoc) {
      delete id;
      return NULL;
    }
    id->setOblique(fontLoc->oblique);
#if LOAD_FONTS_FROM_MEM
    fontBuf = NULL;
    if (fontLoc->fontType == fontType1 ||
	fontLoc->fontType == fontTrueType) {
      if (!(extFontFile = fopen(fontLoc->path->getCString(), "rb"))) {
	delete fontLoc;
	delete id;
	return NULL;
      }
      fontBuf = new GString();
      while ((n = fread(blk, 1, sizeof(blk), extFontFile)) > 0) {
	fontBuf->append(blk, n);
      }
      fclose(extFontFile);
    }
#endif
    if (fontLoc->fontType == fontType1) {
      fontFile = fontEngine->loadType1Font(id,
#if LOAD_FONTS_FROM_MEM
					   fontBuf,
#else
					   fontLoc->path->getCString(),
					   gFalse,
#endif
					   winAnsiEncoding);
    } else if (fontLoc->fontType == fontTrueType) {
#if LOAD_FONTS_FROM_MEM
      if (!(ff = FoFiTrueType::make(fontBuf->getCString(),
				    fontBuf->getLength(),
				    fontLoc->fontNum))) {
#else
      if (!(ff = FoFiTrueType::load(fontLoc->path->getCString(),
				    fontLoc->fontNum))) {
#endif
	delete fontLoc;
	delete id;
	return NULL;
      }
      for (cmap = 0; cmap < ff->getNumCmaps(); ++cmap) {
	cmapPlatform = ff->getCmapPlatform(cmap);
	cmapEncoding = ff->getCmapEncoding(cmap);
	if ((cmapPlatform == 3 && cmapEncoding == 1) ||
	    (cmapPlatform == 0 && cmapEncoding <= 4)) {
	  break;
	}
      }
      if (cmap == ff->getNumCmaps()) {
	delete ff;
	delete fontLoc;
	delete id;
	return NULL;
      }
      codeToGID = (int *)gmallocn(256, sizeof(int));
      for (i = 0; i < 256; ++i) {
	codeToGID[i] = 0;
	if (winAnsiEncoding[i] &&
	    (u = globalParams->mapNameToUnicode(winAnsiEncoding[i]))) {
	  codeToGID[i] = ff->mapCodeToGID(cmap, u);
	}
      }
      delete ff;
      fontFile = fontEngine->loadTrueTypeFont(id,
#if LOAD_FONTS_FROM_MEM
					      fontBuf,
#else
					      fontLoc->path->getCString(),
					      gFalse,
#endif
					      fontLoc->fontNum,
					      codeToGID, 256, NULL);
    } else {
      delete fontLoc;
      delete id;
      return NULL;
    }
    delete fontLoc;
  }
  if (!fontFile) {
    return NULL;
  }

  // create the scaled font
  oblique = ((TileCompositorFontFileID *)fontFile->getID())->getOblique();
  textMat[0] = (SplashCoord)mat[0];
  textMat[1] = (SplashCoord)mat[1];
  textMat[2] = oblique * (SplashCoord)mat[0] + (SplashCoord)mat[2];
  textMat[3] = oblique * (SplashCoord)mat[1] + (SplashCoord)mat[3];
  font = fontEngine->getFont(fontFile, textMat, globalMat);

  return font;
}

#endif // HIGHLIGHTED_REGIONS
