#ifndef ULTIMAOBJECTS_H
#define ULTIMAOBJECTS_H
#include "PAL_platformheader.h"

/*   P R E P R O C E S S O R   M A C R O S   */
#define PointComp(p1,p2)    memcmp(p1,p2,sizeof(POINT))
#define RectComp(r1,r2)     memcmp(r1,r2,sizeof(RECT))


#define min_map_pitch(bounds) ((bounds->right - bounds->left - 1)/16 + 1)



typedef enum _color
{
    WHITE,
    BLACK
} COLOR;

typedef enum _state
{
    UP,
    DOWN
} STATE;

typedef enum _frame_type
{
    INVERT_FRAME,
    SET_FRAME,
    HPATTERN_FRAME,
    CLEAR_FRAME
} FRAME_TYPE;

typedef UINT16 BMWORD;

typedef INT16 HCOORD;

typedef struct _point
{
    HCOORD h;
    HCOORD v;
} UltimaPoint;

typedef struct _rect
{
    HCOORD left;
    HCOORD top;
    HCOORD right;
    HCOORD bottom;
} UltimaRectangle;

typedef struct _pattern
{
    BMWORD data[16];
} *HPATTERN;

typedef struct _pen
{
    UltimaPoint   loc;
    UltimaPoint   size;
    STATE   state;
    COLOR   color;
    HPATTERN *pat;
    BMWORD  setval;
    void   (*drawfunction)();
} UltimaPen;

typedef struct _bitmap
{
    BMWORD* base;
    INT16 pitch;
    UltimaRectangle    bounds;
} UltimaBitMap;

typedef enum _rasterop
{
    ALL_ZEROS,
    S_AND_D,
    S_AND_ND,
    S,
    NS_AND_D,
    D,
    S_XOR_D,
    S_OR_D,
    NS_AND_ND,
    NS_XOR_D,
    ND,
    S_OR_ND,
    NS,
    NS_OR_D,
    NS_OR_ND,
    ALL_ONES
} RASTEROP;

#define DYNAMIC_SIZE            1


/*********************
*  Descriptor Flags  *
*********************/
#define TEXT_ENTITY     0x0001
#define BARCODE_ENTITY  0x0002
#define GRAPHIC_ENTITY  0x0004
#define LINE_ENTITY     0x0008

#define PRE_IMAGE       0x0010
#define POST_IMAGE      0x0020

#define FIXED           0x0100
#define FLOATING        0x0200
#define STRETCH         0x0400
#define VARIABLE        0x0800

#define RECT_BORDER     0x1000 

/****************
*  Option Flags  *
****************/
#define EDIT_IMAGE      0x0001
#define SCRUNCH         0x0002

/***********************
*  Image Status Flags  *
***********************/
#define IMAGEING_ACTIVE     0x0001
#define IMAGING_BARCODE     0x0002
#define IMAGING_TEXT        0x0004
#define IMAGING_GRAPHIC     0x0008
#define ROTATING_90         0x0010
#define ROTATING_180        0x0020
#define ROTATING_270        0x0040
#define INVALID_RECT        0x0080
#define IMVALID_BARCODE     0x0100
#define INVALID_FONT        0x0200
#define INVALID_TEXT        0x0400
#define INVALID_ROTATE      0x0800
#define INVALID_FLOAT       0x1000
#define INVALID_VARIABLE    0x2000
#define IMAGE_LOCKED        0x4000
#define IMAGE_INVALID       0x8000

#endif




