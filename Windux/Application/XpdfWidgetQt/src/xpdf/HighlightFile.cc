//========================================================================
//
// HighlightFile.cc
//
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "gmempp.h"
#include "gfile.h"
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "Error.h"
#include "HighlightFile.h"

//------------------------------------------------------------------------

class HighlightFileTag {
public:

  static HighlightFileTag *read(int (*getChar)(void *stream), void *stream,
				int *cBuf);
  HighlightFileTag(GString *nameA);
  ~HighlightFileTag();
  GString *lookupAttr(const char *attr)
    { return (GString *)attrs->lookup(attr); }

  GString *name;
  GHash *attrs;			// attribute values [GString]
};

static char *getLine(char *buf, int size,
		     int (*getChar)(void *stream), void *stream,
		     int *cBuf) {
  int c, i;

  i = 0;
  while (i < size - 1) {
    if (*cBuf == EOF) {
      *cBuf = (*getChar)(stream);
    }
    c = *cBuf;
    *cBuf = EOF;
    if (c == EOF) {
      break;
    }
    buf[i++] = (char)c;
    if (c == '\x0a') {
      break;
    }
    if (c == '\x0d') {
      *cBuf = (*getChar)(stream);
      if (*cBuf == '\x0a' && i < size - 1) {
	buf[i++] = (char)*cBuf;
	*cBuf = EOF;
      }
      break;
    }
  }
  buf[i] = '\0';
  if (i == 0) {
    return NULL;
  }
  return buf;
}

HighlightFileTag *HighlightFileTag::read(int (*getChar)(void *stream),
					 void *stream, int *cBuf) {
  HighlightFileTag *tag;
  char buf[512];
  char *p0, *p1, *p2;

  if (!getLine(buf, sizeof(buf), getChar, stream, cBuf)) {
    return NULL;
  }
  for (p0 = buf; *p0 && isspace(*p0); ++p0) ;
  if (*p0 != '<') {
    return NULL;
  }
  ++p0;
  for (p1 = p0; *p1 && *p1 !='>' && !isspace(*p1); ++p1) ;
  tag = new HighlightFileTag((new GString(p0, (int)(p1 - p0)))->lowerCase());
  p0 = p1;
  while (1) {
    for (; *p0 && isspace(*p0); ++p0) ;
    if (!*p0 || *p0 == '>') {
      break;
    }
    for (p1 = p0 + 1; *p1 && *p1 != '=' && *p1 != '>' && !isspace(*p1); ++p1) ;
    if (*p1 == '=') {
      for (p2 = p1 + 1; *p2 && *p2 != '>' && !isspace(*p2); ++p2) ;
      tag->attrs->add((new GString(p0, (int)(p1 - p0)))->lowerCase(),
		      new GString(p1 + 1, (int)(p2 - (p1 + 1))));
      p0 = p2;
    } else {
      tag->attrs->add((new GString(p0, (int)(p1 - p0)))->lowerCase(),
		      new GString());
      p0 = p1;
    }
  }
  return tag;
}

HighlightFileTag::HighlightFileTag(GString *nameA) {
  name = nameA;
  attrs = new GHash(gTrue);
}

HighlightFileTag::~HighlightFileTag() {
  delete name;
  deleteGHash(attrs, GString);
}

//------------------------------------------------------------------------

static int getCharFunc(void *stream) {
  return fgetc((FILE *)stream);
}

HighlightFile *HighlightFile::read(char *fileName) {
  HighlightFile *hf;
  FILE *f;

  if (!(f = fopen(fileName, "r"))) {
    error(errIO, -1, "Couldn't open highlight file '{0:s}'", fileName);
    return NULL;
  }
  hf = read(&getCharFunc, f, fileName);
  fclose(f);
  return hf;
}

HighlightFile *HighlightFile::read(int (*getChar)(void *stream),
				   void *stream, const char *fileName) {
  HighlightFile *hf;
  HighlightFileTag *tag;
  GString *val;
  int page, pos, length;
  int cBuf;

  cBuf = EOF;

  // <XML>
  if (!(tag = HighlightFileTag::read(getChar, stream, &cBuf))) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <XML> tag)",
	  fileName);
    goto err1;
  }
  if (tag->name->cmp("xml")) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <XML> tag)",
	  fileName);
    delete tag;
    goto err1;
  }
  delete tag;

  hf = new HighlightFile();

  // <Body units=...>
  if (!(tag = HighlightFileTag::read(getChar, stream, &cBuf))) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <Body> tag)",
	  fileName);
    goto err2;
  }
  if (tag->name->cmp("body")) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <Body> tag)",
	  fileName);
    delete tag;
    goto err2;
  }
  if (!(val = tag->lookupAttr("units"))) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing 'units' value)",
	  fileName);
    delete tag;
    goto err2;
  }
  hf->charUnits = !val->cmp("characters");
  delete tag;

  // <Highlight>
  if (!(tag = HighlightFileTag::read(getChar, stream, &cBuf))) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <Highlight> tag)",
	  fileName);
    goto err2;
  }
  if (tag->name->cmp("highlight")) {
    error(errIO, -1,
	  "File '{0:s}' is not a valid highlight file (missing <Highlight> tag)",
	  fileName);
    delete tag;
    goto err2;
  }
  delete tag;

  while (1) {

    // <loc pg=P pos=O len=L>
    if (!(tag = HighlightFileTag::read(getChar, stream, &cBuf))) {
      break;
    }
    if (!tag->name->cmp("/highlight")) {
      delete tag;
      break;
    }
    if (tag->name->cmp("loc")) {
      error(errIO, -1,
	    "File '{0:s}' is not a valid highlight file (unknown <{1:t}> tag)",
	    fileName, tag->name);
      delete tag;
      break;
    }
    if ((val = tag->lookupAttr("pg"))) {
      page = 1 + atoi(val->getCString());
      if ((val = tag->lookupAttr("pos"))) {
	pos = atoi(val->getCString());
	if ((val = tag->lookupAttr("len"))) {
	  length = atoi(val->getCString());
	  hf->ranges->append(new HighlightRange(page, pos, length));
	} else {
	  error(errIO, -1,
		"File '{0:s}' is not a valid highlight file (missing 'len' value)",
		fileName);
	}
      } else {
	error(errIO, -1,
	      "File '{0:s}' is not a valid highlight file (missing 'pos' value)",
	      fileName);
      }
    } else {
      error(errIO, -1,
	    "File '{0:s}' is not a valid highlight file (missing 'pg' value)",
	    fileName);
    }
    delete tag;
  }

  return hf;

 err2:
  delete hf;
 err1:
  return NULL;
}

HighlightFile::HighlightFile() {
  charUnits = gFalse;
  ranges = new GList();
}

HighlightFile::~HighlightFile() {
  deleteGList(ranges, HighlightRange);
}

int HighlightFile::getLength() {
  return ranges->getLength();
}

HighlightRange *HighlightFile::get(int idx) {
  return (HighlightRange *)ranges->get(idx);
}
