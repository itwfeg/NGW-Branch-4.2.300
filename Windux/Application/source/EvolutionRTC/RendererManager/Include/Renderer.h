#ifndef RENDERER_H
#define RENDERER_H



//#include "vxWorks.h"
#include "CSmartFontHandle.h"
#include "Rectangle.h"



////////////////////////////
/// @name Render Defines
//@{

//
///Word and Byte size constants primarily used 
///with rendering specific calculations
//
#define BYTE_SWAP               			8
#define BYTE_SWAP_ZERO_BASED    			BYTE_SWAP-1
#define NUM_BITS					BYTE_SWAP
#define BMP_IDX_WIDTH                   		16
#define BMP_IDX_WIDTH_ZERO_BASED        		BMP_IDX_WIDTH-1

//@}




class ImageControl;
class FormRecord;
class BitMap;
class LabelStockDescriptor;
class Rectangle;

#define MaxPointSize 16


bool determineRectangleForField(const ImageControl *ic, FormRecord *form, Hobart::Rectangle &theRect);

void erasePostImageFormFields( const ImageControl *ic );

UINT16 renderField(const ImageControl *ic, FormRecord *form, BitMap *pDestBitMap,
		const LabelStockDescriptor* embeddedLSD, bool subtractDescent = false);

#if !defined (PAL_WIN32_RUN)
       /// @name Rendering 
       /// Text rendering.
       //@{
       /// Blits chararcter bmp at the appropriate offset.
	void RenderBuf(
		/// [in] freetype font specific structure containing char bmp details etc.
		FT_GlyphSlot slot, 
		/// [in/out] Word of the buffer where rendering needs to take place.
		FT_Int &off_set, 
		/// [in] x-coordinate where rendering would occur (offset derived from
		/// this and the next var.  This var is also updated in this routine with
		/// the width of the character.
		int32_t str_x, 
		/// [in] y-coordinate where rendering would occur (offset derived from this
		/// and str_x).
		int32_t str_y,
		/// [in] Used as the starting point in y-coordinates within the text rendering
		/// box.  Helps in bottom up rendering and allows letters like 'y' requiring 
		/// rendering below base line to be placed properly.
                int32_t face_height, 
		/// [in] Starting pixel within the word (offset).
		uint8_t starting_bit,
		/// [in,out] buffer to render into
		BitMap &txt_bmp );

	/// Retrives the character glyph, calculates the initial location of rendering
	/// within the buffer and calls appropriate rendering routine to render string
	/// character by character. 
	int32_t TextOut(
		/// [in] handle to the open freetype font lib.
		Util::CSmartFontHandle fhndl,
		/// [in] x-coordinates of the location where rendering must take place
		int32_t str_x, 
		/// [in] y-coordinates of the location where rendering must take place
		int32_t str_y, 
		/// [in] String to render.
        const char * data,
		/// [in] Count of characters to render from the (data) string.
		int32_t count,
		/// [in,out] buffer to render into
		BitMap &txt_bmp );

	//@}
#endif



#endif	//RENDERER_H
