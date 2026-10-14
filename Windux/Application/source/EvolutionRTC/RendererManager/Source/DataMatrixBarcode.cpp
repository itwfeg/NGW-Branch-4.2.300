/**
 * @file DataMatrixBarcode.cpp
 * Data Matrix Barcode renderer adapter.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "DataMatrixBarcode.h"

#include "DrawingPrimitives.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

#include "ErrorHandling.h"
#include "CRendererUtil.h"

#include "../common/rbitmap.h"
#include "../datamatrix/dmapi.h"

static const unsigned char MASK_TABLE[] = {
	0x80, 0x40, 0x20, 0x10,
	0x08, 0x04, 0x02, 0x01 };

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DataMatrixBarcodeAdapter::render(
	BitMap &labelBitmap,
	const Hobart::Rectangle &labelBitmapRect,
	const char *data )
{
	const size_t width( labelBitmapRect.getRectangleRight() - labelBitmapRect.getRectangleLeft() );
	const size_t height( labelBitmapRect.getRectangleBottom() - labelBitmapRect.getRectangleTop() );

	const std::string s( data );

	Debug::trace( "Renderer.DataMatrix", "barcode loc: " +
		Integer::toString( labelBitmapRect.getRectangleLeft() ) + "," +
		Integer::toString( labelBitmapRect.getRectangleTop() ) + "+" +
		Integer::toString( width ) + "+" +
		Integer::toString( height ) );
	Debug::trace( "Renderer.DataMatrix", "  data: " + s );

	bitmap b( width, height );

	// set FNC1
	dmapi::setFNC1( 1 );

	// encode data to bitmap
    if ( 0 <= dmapi::encode( s, dm::ECC200, b, 0, 0, width, height ) )
    {
    	unsigned char *buffer( (unsigned char*) labelBitmap.getBitMapBase() );

    	// for all lines
    	for ( size_t y( height ); y--; )
    	{
    		const size_t offset( 2 * labelBitmap.getBitMapPitch() * (labelBitmapRect.getRectangleTop() + y) );

    		// for each x,y pixel if set
        	for ( size_t x( width ); x--; )
        		if ( b.pixel( x, y ) )
				{
					const size_t bit( labelBitmapRect.getRectangleLeft() + x );

					// mask pixel in buffer on
        			buffer[offset + (bit / 8)] |= MASK_TABLE[bit % 8];
				}

    	}

    	return true;
    }

	Debug::trace( "Renderer.DataMatrix", "render failed!" );
	return false;
}
