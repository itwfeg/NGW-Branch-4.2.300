//========================================================================
//
// HighlightFile.h
//
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef HIGHLIGHTFILE_H
#define HIGHLIGHTFILE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"

class GList;

//------------------------------------------------------------------------

class HighlightRange {
public:

  HighlightRange(int pageA, int posA, int lengthA)
    { page = pageA; pos = posA; length = lengthA; }
  ~HighlightRange() {}

  int page;
  int pos;
  int length;
};

//------------------------------------------------------------------------

class HighlightFile {
public:

  static HighlightFile *read(char *fileName);
  static HighlightFile *read(int (*getChar)(void *stream), void *stream,
			     const char *fileName);
  ~HighlightFile();

  GBool unitsAreCharacters() { return charUnits; }
  int getLength();
  HighlightRange *get(int idx);

private:

  HighlightFile();

  GBool charUnits;		// true for units=characters,
				//   false for units=words
  GList *ranges;		// list of ranges [HighlightRange]
};

#endif
