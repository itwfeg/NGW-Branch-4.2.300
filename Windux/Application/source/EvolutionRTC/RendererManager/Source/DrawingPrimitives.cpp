#include "PAL_platformheader.h"

//;#if	!defined(__linux__)  // ***** WARNING: Stub Linux only
// TODO: We'll have to recreate these drawing functions for LINUX sooner or later
// but for now we'll stub the ones we need. $TRS
#include "BitMap.h"
#include "Rectangle.h"
//void eraseRectangle(BitMap * UNUSED(pDestBitMap), Hobart::Rectangle * UNUSED(imageRect), UINT16 UNUSED(fillWord)) {;}
//void rotateImage(BitMap * UNUSED(sourceBitMap), Hobart::Rectangle * UNUSED(sourceRectangle), 
//        BitMap * UNUSED(destinationBitMap), Hobart::Rectangle * UNUSED(destinationRectangle), INT16 UNUSED(angle)) {;}
//;#else

//#include <windowsx.h> // replaced by "PAL" header
#include "ErrorHandling.h"

#include "Point.h"
#include "Label.h"
#include "Pen.h"
#include "BitMap.h"
#include "Rectangle.h"
#include "WordWrapLineMetrics.h"
#include "GenericBarcode.h"
#include "Ean128CBarcode.h"

#include "string.h"

#include "BitBlt.h"
#include "DrawingPrimitives.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"



#if !defined PAL_WIN32_RUN

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/tttables.h>
#include <freetype/fttypes.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftwinfnt.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//////////////////////////////////////////////////////////////////////////////////////////////
//GetTextExtentPoint32A
//
// FT-495 Re-wrote this function to perform text metrics as per WINE implementation. I did not
// redo the height (y-axis) computation only the width (x-axis).
//
// WARNING: If you modify this funtion you will also need to change TextOut()
//
bool GetTextExtentPoint32A(
	Util::CSmartFontHandle fhndl,
	char *str,
	int32_t len,
	PAL_SIZE *sz )
{
	FT_Face face;
	FT_GlyphSlot glyph;
	FT_BitmapGlyph realglyph;
	LONG error, n, glyph_index, i, previous;
	ULONGLONG TotalWidth = 0;
	FT_CharMap charmap, found = NULL;
	BOOL use_kerning;
	FT_Render_Mode RenderMode;

	PAL_FT_HANDLE ft_hndl = fhndl.Get();


	if ((NULL == ft_hndl) || (NULL == ft_hndl->face))
	{
		Debug::trace("Renderer.Font", "Invalid handle.");
		return 0;
	}

	sz->cx = 0;
	sz->cy = 0;

	face = ft_hndl->face;

	if (face->charmap == NULL)
	{
		Debug::trace("Renderer.Font", "WARNING: No charmap selected!");
		Debug::trace("Renderer.Font", "This font face has " + util_string::inttostring((int)face->num_charmaps) + "charmaps" );

	    for (n = 0; n < face->num_charmaps; n++)
	    {
	        charmap = face->charmaps[n];
	        Debug::trace("Renderer.Font", "Found charmap encoding: " + util_string::inttostring((int)charmap->encoding));
	        if (charmap->encoding != 0)
	        {
	            found = charmap;
	            break;
	        }
	    }

	    if (! found)
	    {
	    	Debug::trace("Renderer.Font", "WARNING: Could not find desired charmap!");
	    }

	    error = FT_Set_Charmap(face, found);
	    if (error)
	    {
	        Debug::trace("Renderer.Font", "WARNING: Could not set the charmap!");
	    }
	}

	RenderMode = FT_RENDER_MODE_MONO;

	use_kerning = FT_HAS_KERNING(face);
	previous = 0;

	for (i = 0; i < len; i++)
	{
		FT_Glyph GlyphCopy;
		FT_Bitmap AlignedBitmap;

		//
		//ignore non-printable characters and add appropriate
		//width when possible (for e.g. for ' ' char)
		//
		switch (str[i])
		{
		case('\n'):
		case('\t'):
		case('\0'):
			continue;
		}

		glyph_index = FT_Get_Char_Index(face, str[i]);

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
		if (error)
		{
        	Debug::trace("Renderer.Font", "WARNING: Failed to load and render glyph! [index: " + util_string::inttostring((int)glyph_index) + "]" );
			break;
		}

		glyph = face->glyph;

		//
		// this height calculation is from old code
		//
		if (glyph->bitmap.rows > sz->cy)
			sz->cy = glyph->bitmap.rows;

		error = FT_Get_Glyph(glyph, &GlyphCopy);
		if (error)
		{
			Debug::trace("Renderer.Font", "Failure caching glyph.");
		}

		error = FT_Glyph_To_Bitmap(&GlyphCopy, RenderMode, 0, 1);
		if (error)
		{
			FT_Done_Glyph(GlyphCopy);
			Debug::trace("Renderer.Font", "Failure rendering glyph.");
			break;
		}

		realglyph = (FT_BitmapGlyph)GlyphCopy;
		FT_Bitmap_New(&AlignedBitmap);
		if(FT_Bitmap_Convert(glyph->library, &realglyph->bitmap, &AlignedBitmap, 4))
		{
			Debug::trace("Renderer.Font", "Conversion failed");
			FT_Done_Glyph((FT_Glyph)realglyph);
			break;
		}

		FT_Bitmap_Done(glyph->library, &realglyph->bitmap);
		realglyph->bitmap = AlignedBitmap;

	    // Retrieve kerning distance
	    if (use_kerning && previous && glyph_index)
	    {
	        FT_Vector delta;
	        FT_Get_Kerning(face, previous, glyph_index, 0, &delta);
	        TotalWidth += delta.x;
	    }

	    TotalWidth += realglyph->root.advance.x >> 10;

	    previous = glyph_index;

	    // free glyph
	    FT_Done_Glyph((FT_Glyph)realglyph);
	}

	sz->cx = (TotalWidth + 32) >> 6;
/*
	sz->cy = (TextObj->logfont.elfEnumLogfontEx.elfLogFont.lfHeight == 0 ?
	            dc->ppdev->devinfo.lfDefaultFont.lfHeight : abs(TextObj->logfont.elfEnumLogfontEx.elfLogFont.lfHeight));
	sz->cy = EngMulDiv(sz->cy, dc->ppdev->gdiinfo.ulLogPixelsY, 72);
*/

/*
	PAL_FT_HANDLE ft_hndl = fhndl.Get();
	FT_GlyphSlot slot;
	int32_t n;
	FT_Error error;

	if ((NULL == ft_hndl) || (NULL == ft_hndl->face))
	{
		Debug::trace("Renderer.Font", "Invalid handle.");
		return 0;
	}
	
	sz->cx = 0;
	sz->cy = 0;
			
	FT_Face face = ft_hndl->face;

	slot = face->glyph;
	for (n = 0;n<len;n++)
	{

		//
		//ignore non-printable characters and add appropriate
		//width when possible (for e.g. for ' ' char)
		//
		switch(str[n])
		{
			case(' '):
//.				sz->cx += (slot->advance.x>>6)/2;
//..				sz->cx += ((face->size->metrics.max_advance>>6)/3-1);
				sz->cx += WHITE_SPACE_ADVANCE(face->size->metrics.max_advance);
			case('\n'):
			case('\t'):
			case('\0'):
				continue;
		}	
		//
		//load glyph image into the slot (erase previous one) 
		//
		error = FT_Load_Char(face, str[n], FT_LOAD_RENDER);
		if (error)
		{
			Debug::trace("Renderer.Font", "Unable to create new font face.");
			return 0;
		}

		//
		//calculate the new string width.  The height does 
		//not need to be recalculated for each char
		//
#ifdef WORD_WRAP_PROB
		sz->cx += slot->bitmap.width;
#else

///
//		if (n != 0)
///
			sz->cx += (slot->advance.x>>6) + (slot->metrics.horiBearingX>>6);
///
//		else
//			sz->cx += (slot->advance.x>>6);
////
#endif
		if (slot->bitmap.rows > sz->cy)
			sz->cy = slot->bitmap.rows;
	}
*/
	return 1;
}
#endif//PAL_WIN32_RUN


static INT16 truncateWordOnEOL(int xStart,
								  int xMax,
								  int wordSize,
								  Util::CSmartFontHandle fontId,
								  WordWrapLineMetrics *wwlm,
								  char *textStart);

static UINT16 readInFontChange(unsigned char *sourceBuffer, unsigned char *newFont);

static bool isWordTerminator(INT8 character);

//will need to place the ultima's line function here also barcode drawing function ect.

/* This function will draw a point the color and size of the pen at	*
 * the specified point in the specified bitmap. 					*/

void drawPoint(Point *point, BitMap *bitmap, Pen *pen)
{
	//create new rectangle to the dimensions of the pen

	//define new right point of the rectangle
	INT16 sizeHoriz = point->getHorizontal() + pen->getPenSizeHorizontal();

	//define the new bottom point of the rectangle 
	INT16 sizeVertical = point->getHorizontal() + pen->getPenSizeVertical(); 

	Hobart::Rectangle rect(point->getVertical(), sizeVertical, sizeHoriz, point->getHorizontal() );

	//make sure the point we are going to draw is within the boundry of the bitmap
	if(bitmap->findPointInBounds(point) != true)
	{
		if (Debug::isTracing("DrawingPrimitives.Error") == true)
		{
			Debug::trace("DrawingPrimitives.Error", "Cannot find point in rectangle");
			point->show();
			bitmap->show();
		}

		return;
	}

	//convert from dominator to Ultima
	UltimaRectangle ultimaRect;
	domRectToUltima(&rect, ultimaRect);

	UltimaBitMap ultimaBitMap;
	domBitMapToUltima(bitmap, ultimaBitMap);

	UINT16 color = pen->getSetVal();

	//write one dot to the bitmap
	RectSetOP(&ultimaBitMap, &ultimaRect, color);
														 
}

void drawLine(Point *startPoint, Point *endPoint, BitMap *bitmap, Pen *pen)
{

Point point;
Point localStart;
Point localEnd;

//INT16 a, b;
//INT16 dxDiag, dyDiag;
//INT16 dxNonDiag, dyNonDiag;
//INT16 d, i;
//INT16 diagInc;
//INT16 nonDiagInc;
//INT16 swap;

/*    extern BOOL ClipLine();
#ifdef LINE_CLIPPING
    if (ClipLine(&localstart,&localend,&bitmap->bounds) == FALSE)
    {
        return;
    }
#endif
*/
	localStart.setHorizontal(startPoint->getHorizontal());
	localStart.setVertical(startPoint->getVertical());

	localEnd.setHorizontal(endPoint->getHorizontal());
	localEnd.setVertical(endPoint->getVertical());
	
	point.setHorizontal((localStart.getHorizontal() + 1));
	point.setVertical((localStart.getVertical() + 1)); 

	if (Debug::isTracing("DrawingPrimitives.DrawLine") == true)
	{
		Debug::trace("DrawingPrimitives.DrawLine", "drawing line from:");
		localStart.show();
		Debug::trace("DrawingPrimitives.DrawLine", "to:");
		localEnd.show();
	}

	//determine line direction vertical or horizontal
	if(localEnd.getHorizontal() == localStart.getHorizontal())
	{
		//line is vertical
  		drawVerticalLine(&localStart, &localEnd, bitmap, pen);
		return;
	}

	if(localEnd.getVertical() == localStart.getVertical())
	{
		//line is horizontal
		drawHorizontalLine(&localStart, &localEnd, bitmap, pen);
		return;
	}
/* //sjjc Isn't all this unnecessary?
	a = localEnd.getHorizontal() - localStart.getHorizontal() - 1;
	b = localEnd.getVertical() - localStart.getVertical() - 1;

	//determine whether ending point lies to right or left of start point 
	if (a < 0)
    {
		  a = -a;
        dxDiag = -1;	//to the left, third or fourth quadrant  
    }
	 else
	{
        dxDiag = 1;	//to the right, first or second quadrant 
    }

	//determine whether ending point lies above or below start point
	if (b < 0)
    {
        b = -b;			//below, second or third quadrant
        dyDiag = -1;
    }
    else
    {
        dyDiag = 1;	//above, first or fourth quadrant
    }

    //  Identify octant containing ending point.  
    if (a < b)
    {
        swap = a;	//we need to use positive numbers
        a = b;
        b = swap;
        dxNonDiag = 0;
        dyNonDiag = dyDiag;
    }
    else
    {
        dxNonDiag = dxDiag;
        dyNonDiag = 0;
    }

    nonDiagInc = b << 1;
	 d = nonDiagInc - a;
    diagInc = d - a;

	 for (i=0; i<=a; ++i)
    {
        drawPoint(&point, bitmap, pen);
        if (d < 0)
        {
            point.addToHorizontal(dxNonDiag);
            point.addToVertical(dyNonDiag);
            d += nonDiagInc;
        }
        else
        {
            point.addToHorizontal(dxDiag);
            point.addToVertical(dyDiag);
            d += diagInc;
        } 
    }
*/
}

/*	This function will draw a vertical line in the specified bitmap, the *
 *	width of the pen from the start point to the end point.				 */
void drawVerticalLine(Point *start, Point *end, BitMap *bitMap, Pen *pen)
{
	//create a new rectangle to the dimensions of the line we are drawing	
#ifdef PAL_WIN32_RUN
	Hobart::Rectangle rectangle(min(start->getVertical(), end->getVertical()),
						max(start->getVertical(), end->getVertical()), 
						max(start->getHorizontal(), end->getHorizontal()) + pen->getPenSizeHorizontal(),
						min(start->getHorizontal(), end->getHorizontal()));
#else
	Hobart::Rectangle rectangle(MIN(start->getVertical(), end->getVertical()),
						std::max(start->getVertical(), end->getVertical()), 
						std::max(start->getHorizontal(), end->getHorizontal()) + pen->getPenSizeHorizontal(),
						std::min(start->getHorizontal(), end->getHorizontal()));
#endif

	//clip the rectangle to the bounds of the bitmap
	Hobart::Rectangle tempRect(Hobart::Rectangle(bitMap->getBitMapBounds()));
	rectangle.clipRect(&tempRect);

	//convert the dom classes to ultima for the bloody RectSetOP function
	UltimaBitMap ultimaBitMap;
	domBitMapToUltima(bitMap, ultimaBitMap);

	UltimaRectangle ultimaRectangle;
	domRectToUltima(&rectangle, ultimaRectangle);

/* not needed see above ats 06242002
	//this should have been done in the ultima code, with out the blitter will never draw the line
	//if this is to be modified make sure that we add this in
	 pUltimarectangle->right += pen->getPenSizeHorizontal();
*/
	// draw this sucker!!!
    RectSetOP(&ultimaBitMap, &ultimaRectangle, pen->getSetVal());
}

void drawHorizontalLine(Point *start, Point *end, BitMap *bitmap, Pen *pen)
{
/* This function will draw a horizontal line in the specified bitmap, the *
 * width of the pen from the start point to the end point. 				  */

	//create a new rectangle to the dimensions of the line we are drawing
#ifdef PAL_WIN32_RUN
	Hobart::Rectangle rectangle(min(start->getVertical(), end->getVertical()),
						max(start->getVertical(), end->getVertical()) + pen->getPenSizeVertical(),
						max(start->getHorizontal(), end->getHorizontal()),
						min(start->getHorizontal(), end->getHorizontal())); 
#else
	Hobart::Rectangle rectangle(std::min(start->getVertical(), end->getVertical()),
						std::max(start->getVertical(), end->getVertical()) + pen->getPenSizeVertical(),
						std::max(start->getHorizontal(), end->getHorizontal()),
						std::min(start->getHorizontal(), end->getHorizontal())); 
#endif

	//clip the rectangle to the bounds of the bitmap
	Hobart::Rectangle tempRect(Hobart::Rectangle(bitmap->getBitMapBounds()));
	rectangle.clipRect(&tempRect);

	//convert the dom classes to ultima for the bloody RectSetOP function
	UltimaBitMap ultimaBitMap;
	domBitMapToUltima(bitmap, ultimaBitMap);

	UltimaRectangle ultimaRectangle;
	domRectToUltima(&rectangle, ultimaRectangle);

	UINT16 color = pen->getSetVal();

	// draw this sucker!!!
     RectSetOP(&ultimaBitMap, &ultimaRectangle, color);
}

void eraseRectangle(BitMap *pDestBitMap, Hobart::Rectangle *imageRect, UINT16 fillWord)
{
	//we have to convert the dominator classes to the ultima types in order	to use the blitting functions
	UltimaBitMap ultimaDestinationBitMap;
	domBitMapToUltima(pDestBitMap, ultimaDestinationBitMap);
	
	UltimaRectangle	ultimaDestinationRectangle;
	domRectToUltima(imageRect, ultimaDestinationRectangle);

/*	Debug::addTraceCategory("DrawingPrimitives.EraseRectangle");

	if (Debug::isTracing("DrawingPrimitives.EraseRectangle") == true)
	{
		imageRect->show();
		pDestBitMap->show();

		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("fillWord:") +	
														 HobString(Integer::toString(fillWord)));

		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.base: 0x") +
														 HobString(Hex::toString((UINT32) ultimaDestinationBitMap.base)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.pitch: ") +
														 HobString(Integer::toString(ultimaDestinationBitMap.pitch)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.bounds.left: ") +
														 HobString(Integer::toString(ultimaDestinationBitMap.bounds.left)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.bounds.right: ") +
														 HobString(Integer::toString(ultimaDestinationBitMap.bounds.right)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.bounds.top: ") +
														 HobString(Integer::toString(ultimaDestinationBitMap.bounds.top)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationBitMap.bounds.bottom: ") +
														 HobString(Integer::toString(ultimaDestinationBitMap.bounds.bottom)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationRectangle.left: ") +
														 HobString(Integer::toString(ultimaDestinationRectangle.left)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationRectangle.right: ") +
														 HobString(Integer::toString(ultimaDestinationRectangle.right)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationRectangle.top: ") +
														 HobString(Integer::toString(ultimaDestinationRectangle.top)));
		Debug::trace("DrawingPrimitives.EraseRectangle", HobString("ultimaDestinationRectangle.bottom: ") +
														 HobString(Integer::toString(ultimaDestinationRectangle.bottom)));
	} */

	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, fillWord);

/*	Debug::removeTraceCategory("DrawingPrimitives.EraseRectangle"); */
}

void rotateImage(BitMap *sourceBitMap, Hobart::Rectangle *sourceRectangle, BitMap *destinationBitMap, Hobart::Rectangle *destinationRectangle, INT16 angle)
{
	/*lcg rev 0.1Debug::trace("DrawingPrimitives.rotateImage", "Source bitmap ((" +
			inttostring(sourceBitMap->getBitMapBoundsLeft()) + "," + inttostring(sourceBitMap->getBitMapBoundsTop()) + "),(" +
			inttostring(sourceBitMap->getBitMapBoundsRight()) + "," + inttostring(sourceBitMap->getBitMapBoundsBottom()) + "))");

	Debug::trace("DrawingPrimitives.rotateImage", "Source rectangle ((" +
			inttostring(sourceRectangle->getRectangleLeft()) + "," + inttostring(sourceRectangle->getRectangleTop()) + "),(" +
			inttostring(sourceRectangle->getRectangleRight()) + "," + inttostring(sourceRectangle->getRectangleBottom()) + "))");

	Debug::trace("DrawingPrimitives.rotateImage", "Destination bitmap ((" +
			inttostring(destinationBitMap->getBitMapBoundsLeft()) + "," + inttostring(destinationBitMap->getBitMapBoundsTop()) + "),(" +
			inttostring(destinationBitMap->getBitMapBoundsRight()) + "," + inttostring(destinationBitMap->getBitMapBoundsBottom()) + "))");

	Debug::trace("DrawingPrimitives.rotateImage", "Destination rectangle ((" +
			inttostring(destinationRectangle->getRectangleLeft()) + "," + inttostring(destinationRectangle->getRectangleTop()) + "),(" +
			inttostring(destinationRectangle->getRectangleRight()) + "," + inttostring(destinationRectangle->getRectangleBottom()) + "))");

	Debug::trace("DrawingPrimitives.rotateImage", "Rotation " + inttostring(angle));
	*/
	//we have to convert the dominator classes to the ultima types in order	to use the blitting functions
	UltimaBitMap ultimaSourceBitMap;
	domBitMapToUltima(sourceBitMap, ultimaSourceBitMap);

	UltimaBitMap ultimaDestinationBitMap;
	domBitMapToUltima(destinationBitMap, ultimaDestinationBitMap);

	UltimaRectangle ultimaSourceRectangle;
	domRectToUltima(sourceRectangle, ultimaSourceRectangle);

	UltimaRectangle	ultimaDestinationRectangle;
	domRectToUltima(destinationRectangle, ultimaDestinationRectangle);

	if (Debug::isTracing("DrawingPrimitives.Rotate") == true)
	{
		Debug::trace("DrawingPrimitives.Rotate",
				"Going to rotate rectangles " + HobString(Integer::toString(angle)) + " degrees");
		Debug::trace("DrawingPrimitives.Rotate", "Source rectangle:");
		sourceRectangle->show();
		Debug::trace("DrawingPrimitives.Rotate", "Destination rectangle:");
		destinationRectangle->show();
	}

	switch(angle)
	{
		case 0:
		{
			//copy the source bitmap to the destination using the coordinates of the source and destination rectangles
			//to determine were the image is to be copied into the destination rectangle
			CopyOP(&ultimaSourceBitMap, &ultimaSourceRectangle, &ultimaDestinationBitMap, &ultimaDestinationRectangle);  
			break;
		}
		case 90:
		{
			//copy the source bitmap to the destination rotated by 90 degrees using the coordinates of the source and destination
		    //rectangles to determine were the image is to be copied into the destination rectangle
	
			Rotate90(&ultimaSourceBitMap, &ultimaSourceRectangle, &ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break;		
		}
		case 180:
		{
			Rotate180(&ultimaSourceBitMap, &ultimaSourceRectangle, &ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break;		
		}
		case 270:
		{
			Rotate270(&ultimaSourceBitMap, &ultimaSourceRectangle, &ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break;
		}
		default:
		{
			Debug::trace("DrawingPrimitives.Error", HobString("rotateImage -- fieldRotation Error:") +
													HobString(Integer::toString(angle)));
		}
	}
} 

void fillRectangle(BitMap *bitMap, Hobart::Rectangle *rectangle, INT16 color)
{
	if (Debug::isTracing("DrawingPrimitives.FillRectangle") == true)
	{
		Debug::trace("DrawingPrimitives.FillRectangle", HobString("color:") + HobString(Integer::toString(color)));
		rectangle->show();
		bitMap->show();
	}
	//convert the dominator classes to ultima data types
	UltimaBitMap ultimaDestinationBitMap;
	domBitMapToUltima(bitMap, ultimaDestinationBitMap);

	UltimaRectangle	ultimaDestinationRectangle;
	domRectToUltima(rectangle, ultimaDestinationRectangle);

	/* This function will fill a rectangle in a bitmap with the color black */
	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, color);
}

void drawFrame(BitMap *bitMap, Hobart::Rectangle *rectangle, FrameType type, Pen *pen)
{
	/* draw the top of the frame */

	//create a horizontal line with the thickness of the pen
	Hobart::Rectangle drawingRectangle(rectangle->getRectangleTop(),
							   rectangle->getRectangleTop() + pen->getPenSizeVertical(),
							   rectangle->getRectangleRight(),
							   rectangle->getRectangleLeft());
		
	//create Ultima types from Dominator classes
	UltimaBitMap ultimaDestinationBitMap;
	domBitMapToUltima(bitMap, ultimaDestinationBitMap);

	UltimaRectangle	ultimaDestinationRectangle;
	domRectToUltima(&drawingRectangle, ultimaDestinationRectangle);

	switch(type)
	{
		case InvertFrame:
		{
			//draw a white line 
			InvertOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break; 
		}
		case SetFrame:
		{
			//draw the line using the color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)pen->getPenColor());
		 	break; 
		}
		case ClearFrame:
		{
			//draw a line with the invert color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)~(pen->getPenColor()));
			break;
		}
		default:
		{
			Debug::trace("DrawingPrimitives.Error", HobString("drawFrame -- Unknown FrameType:") +
													HobString(Integer::toString((UINT32) type)));
			break;
		}
	}

	/* draw the bottom of the frame, scrunch the top to the bottom the thickness of the pen */
	drawingRectangle.setRectangleTop((rectangle->getRectangleBottom() - pen->getPenSizeVertical()));
	drawingRectangle.setRectangleBottom(rectangle->getRectangleBottom());

	domRectToUltima(&drawingRectangle, ultimaDestinationRectangle);

	switch(type)
	{
		case InvertFrame:
		{
			//draw a white line 
			InvertOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break; 
		}
		case SetFrame:
		{
			//draw the line using the color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)pen->getPenColor());
		 	break; 
		}
		case ClearFrame:
		{
			//draw a line with the invert color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)~(pen->getPenColor()));
			break;
		}
		default:
		{
			Debug::trace("DrawingPrimitives.Error", HobString("drawFrame -- Unknown FrameType:") +
													HobString(Integer::toString((UINT32) type)));
			break;
		}
	}
	
	/* draw the right side of the frame */
  	drawingRectangle.setRectangleTop((rectangle->getRectangleTop() + pen->getPenSizeVertical()));
	drawingRectangle.setRectangleBottom((rectangle->getRectangleBottom() - pen->getPenSizeVertical()));
	drawingRectangle.setRectangleLeft((rectangle->getRectangleRight()	- pen->getPenSizeHorizontal())); 	
		
	domRectToUltima(&drawingRectangle,ultimaDestinationRectangle);

	switch(type)
	{
		case InvertFrame:
		{
			//draw a white line 
			InvertOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break; 
		}
		case SetFrame:
		{
			//draw the line using the color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)pen->getPenColor());
		 	break; 
		}
		case ClearFrame:
		{
			//draw a line with the invert color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)~(pen->getPenColor()));
			break;
		}
		default:
		{
			Debug::trace("DrawingPrimitives.Error", HobString("drawFrame -- Unknown FrameType:") +
													HobString(Integer::toString((UINT32) type)));
			break;
		}
	}
	
	/* draw the left of the frame */
	//scrunch the right side of the rectangle from the left the thickness of the pen 
	drawingRectangle.setRectangleLeft(rectangle->getRectangleLeft());
	drawingRectangle.setRectangleRight((rectangle->getRectangleLeft() + pen->getPenSizeHorizontal()));

	domRectToUltima(&drawingRectangle, ultimaDestinationRectangle);

	switch(type)
	{
		case InvertFrame:
		{
			//draw a white line 
			InvertOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle);
			break; 
		}
		case SetFrame:
		{
			//draw the line using the color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)pen->getPenColor());
		 	break; 
		}
		case ClearFrame:
		{
			//draw a line with the invert color of the pen
		 	RectSetOP(&ultimaDestinationBitMap, &ultimaDestinationRectangle, (UINT16)~(pen->getPenColor()));
			break;
		}
		default:
		{
			Debug::trace("DrawingPrimitives.Error", HobString("drawFrame -- Unknown FrameType:") +
													HobString(Integer::toString((UINT32) type)));
			break;
		}
	}
}

void drawGuard(BitMap *bitmap, GenericBarcode *barcode)
{
	drawModule(bitmap, barcode, DarkModule,  1, barcode->getGuardHeight());
	drawModule(bitmap, barcode, LightModule, 1, barcode->getGuardHeight());
	drawModule(bitmap, barcode, DarkModule,  1, barcode->getGuardHeight());
}

void drawMargin(BitMap *bitmap, GenericBarcode *barcode, INT16 rotation)
{
	if(rotation == 0)
	{
		drawModule(bitmap, barcode, LightModule, 7, barcode->getGuardHeight()); 
	}
	else
	{
		//rotation 270 not ready yet
		Debug::trace("DrawingPrimitives.Error", "rotation of barcode not ready yet");
	}
}


#ifdef PAL_WIN32_RUN
void drawSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc)
#else
void drawSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle fhndl, BitMap &txt_bmp)
#endif
{
	//draws the left and right side symbols on the barcode

/*currently we have no container in the FormRecord class for the barcode character data
  we will need to change the FormRecord class to add a container. The Ultima class used a (void *)
  for the graphic & character data */

//printf("current barcode data is %s\n", data);


	data += start;

    for(int i = 0; i < count; i++)
    {
        if ((*data < '0') || (*data > '9'))
        {
			Debug::trace("DrawingPrimitives.Error", HobString("drawSymbols -- symbol out of range:") +
													HobString(Integer::toString(*data)));
			data++;
			continue;
		}

		Debug::trace("Renderer.BarCode", "Draw Symbols: rendering symbol character '" + std::string((char*)data, 1) +
			"' at (" + util_string::inttostring(barcode->getBarcodePenHorizontal()) + "," +
			util_string::inttostring(barcode->getBarcodePenVertical() + barcode->getSymbolHeight() + 1) + ")");

		//the value of 2 is used for added spacing
#ifdef PAL_WIN32_RUN
		Assert(TextOutA(hdc, barcode->getBarcodePenHorizontal(), barcode->getBarcodePenVertical() + barcode->getSymbolHeight() + 1, (char*)data, 1) != 0, Util::CNTException());

#else//!PAL_WIN32_RUN
		if (TextOut(fhndl, barcode->getBarcodePenHorizontal(), barcode->getBarcodePenVertical() + barcode->getSymbolHeight() + 1, 
															(char*)data, 1, txt_bmp) != 0)
		{
			Debug::trace("Renderer.BarCode", "Draw Symbols: Unable to print text");
		}
#endif//!PAL_WIN32_RUN

		drawSymbol(bitmap, barcode, barcode->getSymbol(barcode->getCurrentTable(), *data++ - '0'));
	}
}


void drawSymbol(BitMap *bitmap, GenericBarcode *barcode, UINT8 symbolCode)
{
	ModuleType currentType, lastType;
	INT16 moduleWidth = 1;
	UINT8 testByte = 0x40;	//for seven bit symbol code

	currentType = (symbolCode & testByte) ? DarkModule : LightModule;
	lastType = currentType;
	testByte = testByte >> 1;

	while(testByte > 0)
	{
		currentType = (symbolCode & testByte) ? DarkModule : LightModule;
		if(currentType == lastType)
		{
			moduleWidth++;
		}
		else
		{
			drawModule(bitmap, barcode, lastType, moduleWidth, barcode->getSymbolHeight());
			lastType = currentType;
			moduleWidth = 1;
		}
		testByte = testByte >> 1;
	}
	drawModule(bitmap, barcode, lastType, moduleWidth, barcode->getSymbolHeight());
}	

void drawCenter(BitMap *bitmap, GenericBarcode *barcode)
{
    drawModule(bitmap, barcode, LightModule, 1, barcode->getGuardHeight());
    drawModule(bitmap, barcode, DarkModule, 1, barcode->getGuardHeight());
    drawModule(bitmap, barcode, LightModule, 1, barcode->getGuardHeight());
    drawModule(bitmap, barcode, DarkModule, 1, barcode->getGuardHeight());
    drawModule(bitmap, barcode, LightModule, 1, barcode->getGuardHeight());

}

void drawModule(BitMap *bitmap, GenericBarcode *barcode, ModuleType type, INT16 width, INT16 height)
{
	INT16 actualWidth = width * barcode->getModuleSize();

	if(type == DarkModule)
	{
		//calculate the rectangle to fill  
		Hobart::Rectangle rectToFill;

		rectToFill.setRectangleTop(barcode->getBarcodePenVertical());
		rectToFill.setRectangleLeft(barcode->getBarcodePenHorizontal());

		INT16 newWidth =  barcode->getBarcodePenHorizontal();
		newWidth = newWidth + actualWidth;
 		rectToFill.setRectangleRight(newWidth);

		INT16 newHeight = barcode->getBarcodePenVertical();
		newHeight = newHeight + height;
		rectToFill.setRectangleBottom(newHeight);

		Debug::trace("Renderer.BarCode", "Draw DarkModule: filling rectangle at (" +
			util_string::inttostring(rectToFill.getRectangleTop()) + "," +
			util_string::inttostring(rectToFill.getRectangleLeft()) + ") through (" +
			util_string::inttostring(rectToFill.getRectangleBottom()) + "," +
			util_string::inttostring(rectToFill.getRectangleRight()) + ")");

		fillRectangle(bitmap, &rectToFill, barcode->getBarcodePenColor());
	}
	//move the pen to the next drawing position
	barcode->moveBarcodePen(actualWidth, 0);	

}


/* ALV - 11/2008 */

UINT16 computeEAN128CChecksum( UINT8 *data, UINT16 count, std::list<size_t> &barcode )
{
	static const size_t CODEA( 0x01 );
	static const size_t CODEB( 0x02 );
	static const size_t CODEC( 0x04 );

	static const size_t CODESET( 0 );
	static const size_t C0( 0 );
	static const size_t C1( 1 );

	static const UINT8 code_a[96] = {
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	};

	static const UINT8 code_b[96] = {
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126
	};

	size_t codeset( CODEC );

	// start in CODEC
	barcode.push_back( 105 );
	barcode.push_back( 102 );

	for ( size_t i( 0 ); i < count; ++i )
	{
		const bool dblwide( (i+1) < count );

		const UINT8 c0( data[i] );
		const UINT8 c1( dblwide ? data[i+1] : 0 );

		//
		// determine codeset
		//

		size_t values[2][4];
		values[C0][CODESET] = values[C1][CODESET] = 0;

		// check CODEC
		if (( '0' <= c0 ) && ( c0 <= '9' ))
			values[C0][CODESET] |= CODEC;

		if (( dblwide ) && ( '0' <= c1 ) && ( c1 <= '9' ))
			values[C1][CODESET] |= CODEC;

		for ( size_t j( 96 ); j--; )
		{
			// check CODEA
			if ( c0 == code_a[j] )
			{
				values[C0][CODESET] |= CODEA;
				values[C0][CODEA] = j;
			}

			if (( dblwide ) && ( c1 == code_a[j] ))
			{
				values[C1][CODESET] |= CODEA;
				values[C1][CODEA] = j;
			}

			// check CODEB
			if ( c0 == code_b[j] )
			{
				values[C0][CODESET] |= CODEB;
				values[C0][CODEB] = j;
			}

			if (( dblwide ) && ( c1 == code_b[j] ))
			{
				values[C1][CODESET] |= CODEB;
				values[C1][CODEB] = j;
			}
		}

		//
		// encode
		//

		// both CODEC
		if (( CODEC & values[C0][CODESET] ) && ( CODEC & values[C1][CODESET] ))
		{
			if ( CODEC != codeset )
			{
				barcode.push_back( 99 );
				codeset = CODEC;
			}

			barcode.push_back( ((c0 - '0') * 10) + (c1 - '0') );
			++i; // increment 2
		}
		// both CODEA
		else if (( CODEA & values[C0][CODESET] ) && ( CODEA & values[C1][CODESET] ))
		{
			if ( CODEA != codeset )
			{
				barcode.push_back( 101 );
				codeset = CODEA;
			}

			barcode.push_back( values[C0][CODEA] );
			barcode.push_back( values[C1][CODEA] );
			++i; // increment 2
		}
		// both CODEB
		else if (( CODEB & values[C0][CODESET] ) && ( CODEB & values[C1][CODESET] ))
		{
			if ( CODEB != codeset )
			{
				barcode.push_back( 100 );
				codeset = CODEB;
			}

			barcode.push_back( values[C0][CODEB] );
			barcode.push_back( values[C1][CODEB] );
			++i; // increment 2
		}

		// single character different codeset
		else if ( CODEA & values[C0][CODESET] )
		{
			if ( CODEC & codeset )
			{
				barcode.push_back( 101 );
				codeset = CODEA;
			}
			else if ( CODEB & codeset )
				barcode.push_back( 98 );

			barcode.push_back( values[C0][CODEA] );
		}
		else if ( CODEB & values[C0][CODESET] )
		{
			if ( CODEC & codeset )
			{
				barcode.push_back( 100 );
				codeset = CODEB;
			}
			else if ( CODEA & codeset )
				barcode.push_back( 98 );

			barcode.push_back( values[C0][CODEB] );
		}
	}

	//
	// compute checksum
	//

	size_t checksum( 0 );
	size_t pos( 0 );

	for ( std::list<size_t>::const_iterator i( barcode.begin() ); i != barcode.end(); ++i )
	{
		checksum += (*i) * (pos < 2 ? 1 : pos);
		++pos;
	}

	barcode.push_back( checksum % 103 );

	// terminator
	barcode.push_back( 106 );
	barcode.push_back( 107 );

    return (checksum % 103);
}

void drawEAN128CSymbol(BitMap *bitmap, GenericBarcode *barcode, UINT16 symbolCode)
{
    ModuleType mod_type;
    ModuleType last_mod_type;
    UINT16 n_mods = 1;
    UINT16 testbit = 0x400;     /*  Assuming an eleven bit symbol code */
    
    mod_type = (symbolCode & testbit) ? DarkModule : LightModule;
    last_mod_type = mod_type;
    testbit = testbit >> 1;
        
    while(testbit > 0) 
    {
        mod_type = (symbolCode & testbit) ? DarkModule : LightModule;
        if (mod_type == last_mod_type)
        {
            n_mods++;
        }
        else
        {
			drawModule(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
            last_mod_type = mod_type;
            n_mods = 1;
        }
        testbit = testbit >> 1;
    }
    drawModule(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
}

void drawEAN128CSymbol270(BitMap *bitmap, GenericBarcode *barcode, UINT16 symbolCode)
{
    ModuleType mod_type;
    ModuleType last_mod_type;
    UINT16 n_mods = 1;
    UINT16 testbit = 0x400;     /*  Assuming an eleven bit symbol code */
    
    mod_type = (symbolCode & testbit) ? DarkModule : LightModule;
    last_mod_type = mod_type;
    testbit = testbit >> 1;
        
    while(testbit > 0) 
    {
        mod_type = (symbolCode & testbit) ? DarkModule : LightModule;
        if (mod_type == last_mod_type)
        {
            n_mods++;
        }
        else
        {
			//TODO: drawModule270(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
			drawModule(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
            last_mod_type = mod_type;
            n_mods = 1;
        }
        testbit = testbit >> 1;
    }
    //TODO: drawModule270(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
	drawModule(bitmap,barcode,last_mod_type,n_mods,barcode->getSymbolHeight());
}

#ifdef PAL_WIN32_RUN
void drawEAN128CSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc)
#else
void drawEAN128CSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle UNUSED(fhndl))
#endif
{
#ifdef PAL_WIN32_RUN
	UNREFERENCED_PARAMETER(hdc);
#else
	UNREFERENCED_PARAMETER(fhndl);
#endif

	std::list<size_t> values;
	computeEAN128CChecksum( data, count, values );

	for ( std::list<size_t>::const_iterator i( values.begin() ); i != values.end(); ++i )
	{
		const size_t value( *i );

		Debug::trace("DrawingPrimitives.EAN128", Integer::toString( value ));
    	drawEAN128CSymbol( bitmap, barcode, ((Ean128CBarcode *)barcode)->getValueFromEANTable( (INT16) value ) );
	}

}

#ifdef PAL_WIN32_RUN
void drawEAN128CSymbols270(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc)
#else
void drawEAN128CSymbols270(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle UNUSED(fhndl))
#endif
{
#ifdef PAL_WIN32_RUN
	UNREFERENCED_PARAMETER(hdc);
#else
	UNREFERENCED_PARAMETER(fhndl);
#endif

	std::list<size_t> values;
	computeEAN128CChecksum( data, count, values );

	for ( std::list<size_t>::const_iterator i( values.begin() ); i != values.end(); ++i )
	{
		const size_t value( *i );

		Debug::trace("DrawingPrimitives.EAN128", Integer::toString( value ));
    	drawEAN128CSymbol270( bitmap, barcode, ((Ean128CBarcode *)barcode)->getValueFromEANTable( (INT16) value ) );
	}

}

void domRectToUltima(Hobart::Rectangle *domRect, UltimaRectangle &ultimaRect)
{
	ultimaRect.top		= domRect->getRectangleTop();
	ultimaRect.bottom	= domRect->getRectangleBottom();
	ultimaRect.left 	= domRect->getRectangleLeft();
	ultimaRect.right 	= domRect->getRectangleRight();
}

void domBitMapToUltima(BitMap *domBitMap, UltimaBitMap &ultBitMap)
{
	ultBitMap.pitch			= domBitMap->getBitMapPitch();
	ultBitMap.base			= domBitMap->getBitMapBase();
	ultBitMap.bounds.top	= domBitMap->getBitMapBoundsTop();
	ultBitMap.bounds.bottom	= domBitMap->getBitMapBoundsBottom();
	ultBitMap.bounds.left	= domBitMap->getBitMapBoundsLeft();
	ultBitMap.bounds.right	= domBitMap->getBitMapBoundsRight();
}

void domPenToUltima(Pen *domPen, UltimaPen &ultimaPen)
{
	ultimaPen.color		= (COLOR)domPen->getPenColor();
	ultimaPen.loc.h		= domPen->getPenLocationHorizontal();
	ultimaPen.loc.v		= domPen->getPenLocationVertical();
	ultimaPen.setval	= domPen->getSetVal();
	ultimaPen.size.h	= domPen->getPenSizeHorizontal();
	ultimaPen.size.v	= domPen->getPenSizeVertical();
	ultimaPen.state		= (STATE)domPen->getPenState();
	ultimaPen.pat		= 0;
}


////////////////////////////////////////////////////////////////////////////////////////
//GetAndFixTextMetrics
//
#ifdef PAL_WIN32_RUN
void GetAndFixTextMetrics( HDC hdc, TEXTMETRIC *tm )
#else
void GetAndFixTextMetrics( Util::CSmartFontHandle& fhndl, TEXTMETRIC *tm )
#endif
{
	static bool useExternalLeading = false;
	static bool noNegativeInternalLeading = true;

#ifdef PAL_WIN32_RUN
	GetTextMetrics( hdc, tm );
#else
	PAL_FT_HANDLE ft_hndl = fhndl.Get();

	if ( NULL == ft_hndl )
	{
		Debug::trace("Renderer.Font", "Invalid handle.");
//		Assert(1, Util::CNTException());
	}
	else
	{
	    //
		// FT-495
	    // SWICHED CODE TO USE FONT METRICS AS PER WINE
	    //

		FT_WinFNT_HeaderRec winHeader;

	    TT_OS2 *pOS2 = NULL;
        TT_HoriHeader *pHori = NULL;
		FT_WinFNT_HeaderRec *pWin = NULL;

	    FT_Fixed XScale, YScale;
	    int Ascent, Descent;
		FT_Face face = ft_hndl->face;

		XScale = face->size->metrics.x_scale;
		YScale = face->size->metrics.y_scale;

		//
		// retrieve font metrics and windows font header (if available)
		//
	    pOS2 = (TT_OS2*) FT_Get_Sfnt_Table( face, ft_sfnt_os2 );
	    pHori = (TT_HoriHeader*) FT_Get_Sfnt_Table( face, ft_sfnt_hhea );

		if ( 0 == FT_Get_WinFNT_Header( face, &winHeader ) )
			pWin = &winHeader;

		// use windows font information
		if (pWin)
		{
			tm->tmHeight           = pWin->pixel_height;
			tm->tmAscent           = pWin->ascent;
			tm->tmDescent          = tm->tmHeight - tm->tmAscent;
			tm->tmInternalLeading  = pWin->internal_leading;
			tm->tmExternalLeading  = pWin->external_leading;
			tm->tmAveCharWidth     = pWin->avg_width;
			tm->tmMaxCharWidth     = pWin->max_width;
//			tm->tmWeight           = pWin->weight;
			tm->tmOverhang         = 0;
//			tm->tmDigitizedAspectX = pWin->horizontal_resolution;
//			tm->tmDigitizedAspectY = pWin->vertical_resolution;
//			tm->tmFirstChar        = pWin->first_char;
//			tm->tmLastChar         = pWin->last_char;
//			tm->tmDefaultChar      = pWin->default_char + pWin->first_char;
//			tm->tmBreakChar        = pWin->break_char + pWin->first_char;
//			tm->tmItalic           = pWin->italic;
//			tm->tmUnderlined       = FontGDI->Underline;
//			tm->tmStruckOut        = FontGDI->StrikeOut;
//			tm->tmPitchAndFamily   = pWin->pitch_and_family;
//			tm->tmCharSet          = pWin->charset;
		}
		else
		{
			if (pOS2->usWinAscent + pOS2->usWinDescent == 0)
			{
				Ascent = pHori->Ascender;
				Descent = -pHori->Descender;
			}
			else
			{
				Ascent = pOS2->usWinAscent;
				Descent = pOS2->usWinDescent;
			}

			tm->tmAscent = (face->size->metrics.ascender + 32) >> 6; // Units above baseline
			tm->tmDescent = (32 - face->size->metrics.descender) >> 6; // Units below baseline
			tm->tmInternalLeading = (FT_MulFix(Ascent + Descent - face->units_per_EM, YScale) + 32) >> 6;

			tm->tmHeight = tm->tmAscent + tm->tmDescent;

			// MSDN says:
			//  el = MAX(0, LineGap - ((WinAscent + WinDescent) - (Ascender - Descender)))
			tm->tmExternalLeading = MAX(0, (FT_MulFix(pHori->Line_Gap
		                                     - ((Ascent + Descent)
		                                        - (pHori->Ascender - pHori->Descender)),
		                                     YScale) + 32) >> 6);

			tm->tmAveCharWidth = (FT_MulFix(pOS2->xAvgCharWidth, XScale) + 32) >> 6;
			if (tm->tmAveCharWidth == 0)
			{
				tm->tmAveCharWidth = 1;
			}

			// Correct forumla to get the maxcharwidth from unicode and ansi font
			tm->tmMaxCharWidth = (FT_MulFix(face->max_advance_width, XScale) + 32) >> 6;

//			tm->tmWeight = pOS2->usWeightClass;
		    tm->tmOverhang = 0;
//		    tm->tmDigitizedAspectX = 96;
//		    tm->tmDigitizedAspectY = 96;
/*
		    if (face_has_symbol_charmap(face) || (pOS2->usFirstCharIndex >= 0xf000 && pOS2->usFirstCharIndex < 0xf100))
		    {
		        USHORT cpOEM, cpAnsi;

				EngGetCurrentCodePage(&cpOEM, &cpAnsi);
				tm->tmFirstChar = 0;
		        switch(cpAnsi)
		        {
		        case 1257: // Baltic
		            tm->tmLastChar = 0xf8fd;
		            break;
		        default:
		            tm->tmLastChar = 0xf0ff;
		        }
		        tm->tmBreakChar = 0x20;
		        tm->tmDefaultChar = 0x1f;
		    }
		    else
		    {
		        tm->tmFirstChar = pOS2->usFirstCharIndex; // Should be the first char in the cmap
		        tm->tmLastChar = pOS2->usLastCharIndex;   // Should be min(cmap_last, os2_last)

		        if(pOS2->usFirstCharIndex <= 1)
		            tm->tmBreakChar = pOS2->usFirstCharIndex + 2;
		        else if (pOS2->usFirstCharIndex > 0xff)
		            tm->tmBreakChar = 0x20;
		        else
		            tm->tmBreakChar = pOS2->usFirstCharIndex;
		        tm->tmDefaultChar = tm->tmBreakChar - 1;
		    }
		    tm->tmItalic = (Face->style_flags & FT_STYLE_FLAG_ITALIC) ? 255 : 0;
		    tm->tmUnderlined = FontGDI->Underline;
		    tm->tmStruckOut  = FontGDI->StrikeOut;

		    // Yes TPMF_FIXED_PITCH is correct; braindead api
		    if (! FT_IS_FIXED_WIDTH(face))
		    {
		    	tm->tmPitchAndFamily = TMPF_FIXED_PITCH;
		    }
		    else
		    {
		    	tm->tmPitchAndFamily = 0;
		    }

		    switch (pOS2->panose[PAN_FAMILYTYPE_INDEX])
		    {
		    case PAN_FAMILY_SCRIPT:
		        tm->tmPitchAndFamily |= FF_SCRIPT;
		        break;
		    case PAN_FAMILY_DECORATIVE:
		        tm->tmPitchAndFamily |= FF_DECORATIVE;
		        break;

		    case PAN_ANY:
		    case PAN_NO_FIT:
		    case PAN_FAMILY_TEXT_DISPLAY:
		    case PAN_FAMILY_PICTORIAL: // Symbol fonts get treated as if they were text
		                               // Which is clearly not what the panose spec says.
		        if (tm->tmPitchAndFamily == 0) // Fixed
		        {
		            tm->tmPitchAndFamily = FF_MODERN;
		        }
		        else
		        {
		            switch (pOS2->panose[PAN_SERIFSTYLE_INDEX])
		            {
		            case PAN_ANY:
		            case PAN_NO_FIT:
		            default:
		                tm->tmPitchAndFamily |= FF_DONTCARE;
		                break;

		            case PAN_SERIF_COVE:
		            case PAN_SERIF_OBTUSE_COVE:
		            case PAN_SERIF_SQUARE_COVE:
		            case PAN_SERIF_OBTUSE_SQUARE_COVE:
		            case PAN_SERIF_SQUARE:
		            case PAN_SERIF_THIN:
		            case PAN_SERIF_BONE:
		            case PAN_SERIF_EXAGGERATED:
		            case PAN_SERIF_TRIANGLE:
		                tm->tmPitchAndFamily |= FF_ROMAN;
		                break;

		            case PAN_SERIF_NORMAL_SANS:
		            case PAN_SERIF_OBTUSE_SANS:
		            case PAN_SERIF_PERP_SANS:
		            case PAN_SERIF_FLARED:
		            case PAN_SERIF_ROUNDED:
		                tm->tmPitchAndFamily |= FF_SWISS;
		                break;
		            }
		        }
		        break;
		    default:
		        tm->tmPitchAndFamily |= FF_DONTCARE;
		    }

		    if (FT_IS_SCALABLE(face))
		    {
		        tm->tmPitchAndFamily |= TMPF_VECTOR;
		    }
		    if (FT_IS_SFNT(face))
		    {
		        tm->tmPitchAndFamily |= TMPF_TRUETYPE;
		    }

		    tm->tmCharSet = DEFAULT_CHARSET;
*/

		    //
		    // HERE IS THE ORIGINAL CODE
		    //
/*
			tm->tmOverhang = 0;
//.         tm->tmInternalLeading = 0;
//.         tm->tmExternalLeading = 0;

			//
			//get various values and covert them from font units.
			//
			tm->tmMaxCharWidth = face->size->metrics.max_advance/64;
			tm->tmAscent = face->size->metrics.ascender/64;
			tm->tmDescent = abs(face->size->metrics.descender/64);
			tm->tmHeight = face->size->metrics.height/64;

//?????????
//          tm->tmExternalLeading = tm->tmAscent/10;
			//
			//externalleading is essentially line gap.
			//internal leading is for symbols
			//
			tm->tmExternalLeading = tm->tmHeight - tm->tmAscent + tm->tmDescent;
			tm->tmInternalLeading = tm->tmAscent - tm->tmDescent - face->units_per_EM;
//???????????

 */
		}
	}

#endif//if not PAL_WIN32_RUN

	bool isTracing = Debug::isTracing("DrawingPrimitives.Clare");
	if (isTracing)
	{
		Debug::trace("DrawingPrimitives.Clare", "         tm->tmHeight = " + util_string::longtostring(tm->tmHeight));
		Debug::trace("DrawingPrimitives.Clare", "         tm->tmAscent = " + util_string::longtostring(tm->tmAscent));
		Debug::trace("DrawingPrimitives.Clare", "        tm->tmDescent = " + util_string::longtostring(tm->tmDescent));
		Debug::trace("DrawingPrimitives.Clare", "tm->tmInternalLeading = " + util_string::longtostring(tm->tmInternalLeading));
		Debug::trace("DrawingPrimitives.Clare", "tm->tmExternalLeading = " + util_string::longtostring(tm->tmExternalLeading));
		Debug::trace("DrawingPrimitives.Clare", "   tm->tmAveCharWidth = " + util_string::longtostring(tm->tmAveCharWidth));
		Debug::trace("DrawingPrimitives.Clare", "   tm->tmMaxCharWidth = " + util_string::longtostring(tm->tmMaxCharWidth));
		Debug::trace("DrawingPrimitives.Clare", "       tm->tmOverhang = " + util_string::longtostring(tm->tmOverhang));
	}

	if ( !useExternalLeading )
	{
		tm->tmExternalLeading = 0;
		Debug::trace("DrawingPrimitives.Clare", "( !useExternalLeading ):  tm->tmExternalLeading = " + util_string::longtostring(tm->tmExternalLeading));
	}

	if ( noNegativeInternalLeading && tm->tmInternalLeading < 0 )
	{
		tm->tmInternalLeading = 0;
		Debug::trace("DrawingPrimitives.Clare", "( noNegativeInternalLeading && tm->tmInternalLeading < 0 ):  tm->tmInternalLeading = " + util_string::longtostring(tm->tmInternalLeading));
	}
}


INT16 drawWordWrapLine(INT16 xMax, INT16 yMax, WordWrapLineMetrics *wordWrapLineMetrics, bool subtractDescent)
{
	unsigned char *inputText = (unsigned char *) wordWrapLineMetrics->getTextStart();
   	INT16 currentPos = 0;		// current position in chars on line
								// inputText[currentPos] = place in original string
	INT16 x = 0;			// current position in pixels on line
	UINT16 lastCharLocation = 0;
	UINT16 fontAscent = 0;

#if defined PAL_WIN32_RUN
	HDC hdc = wordWrapLineMetrics->getFontDriver();
#endif
	Util::CSmartFontHandle currentFontId = wordWrapLineMetrics->getFontId(0, lastCharLocation, fontAscent);
	TEXTMETRIC tm;

#if defined PAL_WIN32_RUN
	SelectFont(hdc, currentFontId.Get());
#endif

#ifdef PAL_WIN32_RUN 
	GetAndFixTextMetrics(hdc, &tm);
#else
	GetAndFixTextMetrics(currentFontId, &tm);
#endif

	WordStatus status = GetWord;

	bool fontChangeInWord = false;

#if defined PAL_WIN32_RUN
	WordWrapLineMetrics fontChangeInWordMetrics(hdc);
#else
	WordWrapLineMetrics fontChangeInWordMetrics(currentFontId);
#endif

	WordWrapLineMetrics *pOldWWM = &fontChangeInWordMetrics;
	WordWrapLineMetrics *pCurWWM = wordWrapLineMetrics;

	while ( status != EndOfLine && status != Quit )
	{
		INT16 textLength = 0;     // length in pixels of the current word
		UINT16 wordAscent = 0;     // ascent in pixels of the current word
		UINT16 wordDescent = 0;    // descent in pixels of the current word
		UINT16 charCount = 0;   // place to store the characters returned in the word.
		unsigned char fontString[MaxFamilyName + 16] = ""; // {"faceName="}; // place to store the new font string


		status = buildWord(inputText + currentPos, &charCount, fontString);
 	  	switch ( status )
		{
			case FontChangeInWord:
			case FullWord:
			case FullWordTabbed:
			case EndOfLine:
			case Quit:
			{
#ifdef PAL_WIN32_RUN 
				SIZE sz;
#else
				PAL_SIZE sz;
#endif
				wordAscent = (UINT16)(tm.tmAscent - tm.tmExternalLeading);
				wordDescent = (UINT16)tm.tmDescent;

				if ( (status == FontChangeInWord) && (fontChangeInWord == false) )
				{
#if defined PAL_WIN32_RUN
					fontChangeInWordMetrics = WordWrapLineMetrics(hdc);
#else
					fontChangeInWordMetrics = WordWrapLineMetrics(currentFontId);
#endif

					fontChangeInWordMetrics.addFontMetrics(0, currentFontId, wordAscent);

					fontChangeInWord = true;

					pOldWWM = pCurWWM;
					pCurWWM = &fontChangeInWordMetrics;
				}

				//get the characteristics of the word
				bool endsInWhiteSpace = false;
				INT16 shortTextLength = 0;
				char lastChar = (charCount > 0) ? inputText[currentPos + charCount - 1] : '\0';
				if ( lastChar == '\n' || lastChar == ' ' )       // took back off wasnt accounting for size of comma - || lastChar == ',' || lastChar == ';' )
				{
#ifdef PAL_WIN32_RUN 
					GetTextExtentPoint32A(hdc, (char*)&inputText[currentPos], charCount - 1, &sz); 
#else
					GetTextExtentPoint32A(currentFontId, (char*)&inputText[currentPos], charCount - 1, &sz); 
#endif
					shortTextLength = (INT16)(sz.cx - tm.tmOverhang);
					endsInWhiteSpace = true;
				}

#ifdef PAL_WIN32_RUN 
				GetTextExtentPoint32A(hdc, (char*)&inputText[currentPos], charCount, &sz); 
#else
				GetTextExtentPoint32A(currentFontId, (char*)&inputText[currentPos], charCount, &sz); 
#endif
				textLength = (INT16)(sz.cx - tm.tmOverhang);

				if ( sz.cy - tm.tmExternalLeading - (subtractDescent ? tm.tmDescent : 0) <= yMax ) //sjjc added the '=' portion of the operator
				{
					// will the current word fit on this line?
					if ( x + (endsInWhiteSpace ? shortTextLength : textLength) <= xMax )
					{
						if ( status == FullWordTabbed )
						{
							INT16 nextTab = (INT16)(((x + textLength) / 40 + 1) * 40);       // Tabs every 5mm.
							if ( nextTab <= xMax )
//							if ( x + textLength < xMax * 67 / 100 )
							{
								// If the tab happened before the 60% mark then position there
								textLength = nextTab - x;
//								textLength = xMax * 67 / 100 - x;
								Debug::trace("DrawingPrimitives.wordwrap", 20, "Found TAB. Shifting right");
							}
							else
							{
								// If the tab happened afterwards then start a new line
								textLength = xMax - x;
								status = EndOfLine;
								Debug::trace("DrawingPrimitives.wordwrap", 20, "Found TAB. Setting EndOfLine");
							}

						}

						// keep track of were we are on the line
						x = x + textLength;
						currentPos = currentPos + charCount;

						pCurWWM->incLineLengthInPixels((status == EndOfLine || status == Quit) && endsInWhiteSpace ? shortTextLength : textLength);
						pCurWWM->incBlockLengthInPixels((status == EndOfLine || status == Quit) && endsInWhiteSpace ? shortTextLength : textLength);
						pCurWWM->incLineLengthInChars(charCount);
						pCurWWM->newLineHeight(wordAscent, wordDescent, (UINT16)(tm.tmInternalLeading + tm.tmExternalLeading));
						
						if ( status == FullWordTabbed )
						{
							Util::CSmartFontHandle fontId = pCurWWM->getFontId(currentPos, lastCharLocation, wordAscent);
							pCurWWM->addFontMetrics(currentPos, fontId, wordAscent);
						}


						// If we are working with on a word with font changes in it,
						// we need to check if this word will fit on the original line

						if ( (status == FullWord || status == FullWordTabbed || status == EndOfLine) 
								&& (fontChangeInWord == true) )
						{
							UINT16 newLineLength = pCurWWM->getLineLengthInPixels() + pOldWWM->getLineLengthInPixels();

							if (newLineLength <= xMax)
							{
						 		pOldWWM->addFontChangeWordToLine(pCurWWM);
							}
							else
							{
								status = EndOfLine;
							}

							pCurWWM = pOldWWM;

#ifdef PAL_WIN32_RUN 
							fontChangeInWordMetrics = WordWrapLineMetrics(hdc);
#else
							fontChangeInWordMetrics = WordWrapLineMetrics(currentFontId);
#endif
							fontChangeInWord = false;
						}
		   			}
		    		else 
					{
						// done parsing??
						if (status != Quit)
						{
							status = EndOfLine;
						}

						// one word too long to fit on one line??
						if ((fontChangeInWord == false) && ((status == Quit) ||	(currentPos == 0)))
						{
							currentPos = currentPos + truncateWordOnEOL(x,
															xMax,
															charCount,
															currentFontId,
															pCurWWM,
															(char *) &inputText[currentPos]);

						}
						else if ((fontChangeInWord == true) && (pOldWWM->getLineLengthInChars() == 0))
						{
							// just add the entire word
							pCurWWM->incLineLengthInPixels(textLength);
							pCurWWM->incBlockLengthInPixels(textLength);
							pCurWWM->incLineLengthInChars(charCount);
							pCurWWM->newLineHeight(wordAscent, wordDescent, (UINT16)(tm.tmInternalLeading + tm.tmExternalLeading));

							// re-initialize variables.
							x = 0;
							currentPos = 0;

							// swap wordwraplinemetrics
							pCurWWM = pOldWWM;
							pOldWWM = &fontChangeInWordMetrics;

							UINT16 i = 0;
							UINT16 curCharLocation = 0;
							UINT16 lastCharLocation = 0;
							UINT16 numFonts = pOldWWM->getNumFonts();
							bool done = false;

							while ((!done) && (i < numFonts))
							{
								UINT16 fontAscent = 0;
								Util::CSmartFontHandle fontId = pOldWWM->getFontId(curCharLocation, lastCharLocation, fontAscent);
								pCurWWM->addFontMetrics(curCharLocation, fontId, fontAscent);

								UINT16 numChars = lastCharLocation - curCharLocation + 1;

#ifdef PAL_WIN32_RUN 
								GetTextExtentPoint32A(hdc, (char*)&inputText[currentPos], numChars, &sz); 
#else
								GetTextExtentPoint32A(fontId, (char*)&inputText[currentPos], numChars, &sz); 
#endif
								textLength = (INT16)(sz.cx - tm.tmOverhang);
								if ((x + textLength) <= xMax)
								{
									x = x + textLength;
									currentPos = currentPos + numChars;
									curCharLocation = curCharLocation + numChars;

									if (inputText[currentPos] == ESC && inputText[currentPos+1] == ESC)
									{
										currentPos = currentPos + skipFontChangeSequence((const char *) &inputText[currentPos]);
									}

									pCurWWM->incLineLengthInPixels(textLength);
									pCurWWM->incBlockLengthInPixels(textLength);
									pCurWWM->incLineLengthInChars(numChars);
									pCurWWM->newLineHeight(wordAscent, wordDescent, (UINT16)(tm.tmInternalLeading + tm.tmExternalLeading));

									i++;
								}
								else
								{
									currentPos = currentPos + truncateWordOnEOL(x,
																	xMax,
																	numChars,
																	fontId,
																	pCurWWM,
																	(char *) &inputText[currentPos]);
									done = true;
								}
							}
						}
					}
				}
				else //sjjc: removed the comments around this else block
				{
					if (fontChangeInWord == true)
					{
						// swap wordwraplinemetrics
						pCurWWM = pOldWWM;
						pOldWWM = &fontChangeInWordMetrics;
					}

					status = Quit;
				}

				if (status == Quit)
				{
					pCurWWM->setTextAreaFull();
				}
			}
			break;

			case FontChange:
			{
				Util::CSmartFontHandle previousFontId = currentFontId;
#ifdef PAL_WIN32_RUN
				currentFontId = generateFontId(hdc, (const char *) fontString);
#else
				currentFontId = generateFontId((const char *) fontString);
#endif
				if ( currentFontId.null() )
					currentFontId = previousFontId;

#ifdef PAL_WIN32_RUN
				SelectFont(hdc, currentFontId.Get());
#endif
#ifdef PAL_WIN32_RUN 
				GetAndFixTextMetrics(hdc, &tm);
#else
				GetAndFixTextMetrics(currentFontId, &tm);
#endif

				pCurWWM->addFontMetrics(pCurWWM->getLineLengthInChars(), currentFontId, (UINT16)(tm.tmAscent - tm.tmExternalLeading));
				currentPos = currentPos + charCount;
			}
			break;	

			default:
			{
				Debug::trace("DrawingPrimitives.Error", HobString("drawWordWrapLine -- text rendering error:") +
														HobString(Integer::toString((UINT16) status)));
				return currentPos;
			}
			break;
		}
	}

	return currentPos;
}


#ifdef PAL_WIN32_RUN
Util::CSmartFontHandle generateFontId(HDC fontDriver, const char * fontString)
#else
Util::CSmartFontHandle generateFontId(const char * fontString)
#endif
{
	Util::CSmartFontHandle fontId;

#ifdef PAL_WIN32_RUN
	if ( fontDriver != (HDC) NULL )
#endif
	{
		const char * pChar = fontString;
		char faceName[32];
		char * pDest = faceName;
		int pointSize;

		// Skip over "faceName="
		pChar += 9;
		while ( *pChar != '\0' && *pChar != ';' )
			*(pDest++) = *(pChar++);
		*pDest = '\0';

		// Skip over ";pixelSize="
		pChar += 11;
		pointSize = atoi(pChar);

//		sscanf(fontString, "faceName=%s;pixelSize=%d", faceName, &pointSize);
//sjjc		uglFontFindString(fontDriver, fontString, &fontdef);

#ifdef PAL_WIN32_RUN
		fontId = Util::CSmartFontHandle(fontDriver, faceName, pointSize);
#else
		fontId = Util::CSmartFontHandle(faceName, pointSize);
#endif
	}

	return fontId;
}


WordStatus buildWord(unsigned char *sourceBuffer, UINT16 *count, unsigned char *newFont)
{
	WordStatus status = GetWord;

 	// consume a character from the string			
 	INT8 character = *sourceBuffer++;
	UINT16 charCount = 1;

	if (character == ESC && *sourceBuffer == ESC)
	{
		UINT16 x = readInFontChange(sourceBuffer, newFont);

		// adjust sourceBuffer and count for font change sequence
		sourceBuffer += x;
		charCount = charCount + x;
		status = FontChange;
	}
	else
	{
		while (status == GetWord)		//do not allow a single word greater 80 chararters  
		{
			if (character == ESC && *sourceBuffer == ESC)
			{
				charCount--;
				UINT16 x = readInFontChange(sourceBuffer, newFont);

				// check to see if Font Change Sequence is at the end of word.
				if ((isWordTerminator(sourceBuffer[x]) == true) ||
					(sourceBuffer[x] == '\n') ||
					(sourceBuffer[x] == '\0'))
				{
					status = FullWord;
				}
				else
				{					
					status = FontChangeInWord;
				}
			}
			else if (isWordTerminator(character) == true)
			{
				// return the separator with the word.
				status = FullWord;
			}
			else if (character == '\t')
			{
				// the word ended in tab (return the tab too)
				status = FullWordTabbed;
			}
			else if (character == '\n')
			{
				// Commented out the following line since it appears to cause a problem with
				// text that contains multiple paragraphs ('\n')
				// charCount--;
				status = EndOfLine;
			}
			else if (character == '\0')
			{
				charCount--;

				if (charCount == 0)
				{
					status = Quit;
				}
				else
				{
					status = FullWord;
				}
			}
			else
			{
			 	// consume the next character from the string			
			 	character = *sourceBuffer++;			
				charCount++;
			}
		}
	}

	*count = charCount;

	return (status);
}


// readInFontChange
// reads the font from sourceBuffer and
// stores it in newFont.
// newFont space must already be allocated!!
// readInFontChange returns the number of characters consumed in sourceBuffer
UINT16 readInFontChange(unsigned char *sourceBuffer, unsigned char *newFont)
{
	const char *faceNameString = "faceName="; // place to store the new font string
	const char *pixelSizeString = "pixelSize=";
	unsigned char *pChar = sourceBuffer;
	UINT16 count = 0;

	//skip over the second esc and "fn="
	pChar += 4;
	count += 4;

	strcpy((char *) newFont, faceNameString);

	size_t i = strlen(faceNameString);

   	while(*pChar != ';')
   	{
   	 	newFont[i++] = *pChar++;
		count++;
   	}

	//get the ";"
		newFont[i++] = *pChar++;
	count++;

	//append null for strcat
	newFont[i] = '\0';
	strcat((char *) newFont, pixelSizeString);		  

	//skip over "ps="
	pChar += 3;
	count += 3;

	i += strlen(pixelSizeString);		//over write the appended null terminator
   	while (*pChar != ESC)
   	{
   	 	newFont[i++] = *pChar++;
		count++;
   	}

	newFont[i] = '\0';

	//skip over the last esc
   	pChar++;
	count++;

	return count;
}

bool isWordTerminator(INT8 character)
{
	bool rv = false;

	switch (character)
	{
		// Whitespace checks
		case ' ':					// 32
		// Word Terminator Tests
		case '-':					// 45
		case '.':					// 46
		case ':':					// 58
		case ',':					// 44
		case ';':					// 59
					rv = true;
					break;
		default:
					break;
	}

	return rv;
}
				

UINT16 skipFontChangeSequence(const char *pChar)
{
	UINT16 i = 0;

	i++;
	while (pChar[i] != ESC)
	{
		i++;
	}

	i++;
	while (pChar[i] != ESC)
	{
		i++;
	}
	i++;
	
	return i;
}


static INT16 truncateWordOnEOL(int xStart,
								  int xMax,
								  int wordSize,
								  Util::CSmartFontHandle fontId,
								  WordWrapLineMetrics *wwlm,
								  char *textStart)
{
	INT16 textLength = 0;	// length in pixels of the current word
//	int wordHeight = 0;	// heighth in pixels of the current word
	UINT16 wordAscent = 0;	// ascent in pixels of the current word
	UINT16 wordDescent = 0;	// descent in pixels of the current word
	UINT16 i = 0;
	bool done = false;
#ifdef PAL_WIN32_RUN
	SIZE sz;
	HDC hdc = wwlm->getFontDriver();
#else
	PAL_SIZE sz;
#endif
	TEXTMETRIC tm;


#ifdef PAL_WIN32_RUN 
	GetAndFixTextMetrics(hdc, &tm);
#else
	GetAndFixTextMetrics(fontId, &tm);
#endif
	wordAscent = (UINT16)(tm.tmAscent - tm.tmExternalLeading);
	wordDescent = (UINT16)tm.tmDescent;

	while ((!done) && (i < wordSize))
	{
		//get the characteristics of the word
#ifdef PAL_WIN32_RUN 
		GetTextExtentPoint32A(hdc, &textStart[i], 1, &sz); 
#else
		GetTextExtentPoint32A(fontId, &textStart[i], 1, &sz); 
#endif
		textLength = (INT16)(sz.cx - tm.tmOverhang);

		if ((xStart + textLength) <= xMax)
		{
			xStart += textLength;

			wwlm->incLineLengthInPixels(textLength);
			wwlm->incBlockLengthInPixels(textLength);
			wwlm->incLineLengthInChars();
			wwlm->newLineHeight(wordAscent, wordDescent, (UINT16)(tm.tmInternalLeading + tm.tmExternalLeading));

			i++;
		}
		else
		{
			done = true;
		}
	}

	return i;
}
//;#endif      // __linux__
