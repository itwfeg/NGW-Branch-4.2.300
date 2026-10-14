#ifndef _PAL_GRAPHICS_H_
#define _PAL_GRAPHICS_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include "PAL_typedef.h"





#define PAL_FACESZ		251

enum
{
	PAL_BI_RGB=		0L
	,PAL_BI_RLE8=		1L
	,PAL_BI_RLE4=		2L
	,PAL_BI_BITFIELDS=	3L
	,PAL_BI_JPEG=		4L
	,PAL_BI_PNG=		5L
};



struct __attribute__((__packed__)) PAL_BITMAP_FILE_HEADER
{
	WORD bf_type;
	DWORD bf_size;
	WORD bf_resrvd1;
	WORD bf_resrvd2;
	DWORD bf_off_bits;
};


struct __attribute__((__packed__)) PAL_BITMAP_INFO_HEADER
{
	DWORD bi_size;
	LONG bi_width;
	LONG bi_height;
	WORD bi_planes;
	WORD bi_bit_cnt;
	DWORD bi_compression;
	DWORD bi_size_img;
	LONG bi_xpels_per_meter;
	LONG bi_ypels_per_meter;
	DWORD bi_clr_used;
	DWORD bi_clr_imp;
};


struct __attribute__((__packed__)) PAL_RGB_QUAD
{
	BYTE rgb_blue;
	BYTE rgb_green;
	BYTE rgb_red;
	BYTE rgb_resrvd;
};


struct TEXTMETRIC
{
	LONG tmHeight;
	LONG tmAscent;
	LONG tmDescent;
	LONG tmInternalLeading;
	LONG tmExternalLeading;
	LONG tmOverhang;
	LONG tmMaxCharWidth;
	LONG tmAveCharWidth;
};


struct PAL_SIZE
{
	PAL_SIZE():cx(0), cy(0){}
	LONG cx;
	LONG cy;
};


typedef struct PAL_FT
{
	FT_Face face;
	FT_Library lib;
	int refCount;
}*PAL_FT_HANDLE;





#endif//_PAL_GRAPHICS_H_

