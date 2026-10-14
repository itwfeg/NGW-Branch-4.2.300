#include "BitMap.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"


std::list<UINT16 *> BitMap::bitmapsToDelete;
Util::CSmartCriticalSection BitMap::bitmapsToDeleteCS;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BitMap::dataType = 0;
std::string BitMap::dataTypeName("BitMap"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
BitMap::BitMap() :
	base( NULL ),
	baseSize( 0 ),
	pitch( 0 )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BitMap::~BitMap()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	// delete some bitmaps
	Util::CLockCriticalSection lock( bitmapsToDeleteCS );

	freeBitMap();

	while ( !bitmapsToDelete.empty() )
	{
		delete [] bitmapsToDelete.front();
		bitmapsToDelete.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Hobart::Rectangle BitMap::getBitMapBounds() const
{
	return bounds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::setBitMapBounds(INT16 recLeft, INT16 recRight, INT16 recTop, INT16 recBottom)
{
	bounds.setRectangleLeft(recLeft);
	bounds.setRectangleRight(recRight);
	bounds.setRectangleTop(recTop);
	bounds.setRectangleBottom(recBottom);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
INT16 BitMap::getBitMapBoundsTop() const
{
	return bounds.getRectangleTop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
INT16 BitMap::getBitMapBoundsBottom() const
{
	return bounds.getRectangleBottom();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
INT16 BitMap::getBitMapBoundsRight() const
{
	return bounds.getRectangleRight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
INT16 BitMap::getBitMapBoundsLeft() const
{
	return bounds.getRectangleLeft();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BitMap::findPointInBounds(Point *point)
{
	return bounds.findPointInRectangle(point);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 *BitMap::getBitMapBase()
{
	return base;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::setBitMapBase(UINT16 *bitMapBase, UINT32 numWords)
{
	base = bitMapBase;

	if ( !base )
		baseSize = 0;
	else
		baseSize = numWords;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT16 *BitMap::allocateBitMap(UINT32 size, bool fill, UINT8 fillChar)
{
	base = new UINT16[size];
	baseSize = size;

	if ((base != NULL) && (fill == true))
	{
		//clear the bitmap
		memset(base, fillChar, size * 2);
	}
	
	return base;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::copyBitMap(UINT16 *source, UINT32 numWords)
{
	if ((base != NULL) && (numWords <= baseSize))
	{
		//clear the bitmap
		memcpy(base, (UINT8 *) source, numWords * 2);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::freeBitMap()
{
	if ( !base )
		return;

	// -1 indicates setBitMapBase() was called and we should not delete
	if ( -1 != baseSize )
	{
		// delete some bitmaps
		Util::CLockCriticalSection lock( bitmapsToDeleteCS );

		bitmapsToDelete.push_back( base );

		while ( 16 < bitmapsToDelete.size() )
		{
			delete [] bitmapsToDelete.front();
			bitmapsToDelete.pop_front();
		}
	}

	base = NULL;
	baseSize = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
INT16 BitMap::getBitMapPitch() const
{
	return pitch;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::setBitMapPitch(INT16 mapDelta)
{
	pitch = mapDelta;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::show() const
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("BitMap") );
#else
	Debug::addTraceCategory("BitMap");
#endif
	Debug::trace("BitMap", "BitMap: ");

	bounds.show();
	Debug::trace("BitMap", HobString("base: 0x") +
						   HobString(Hex::toString((long long) base)));
	Debug::trace("BitMap", HobString("pitch: ") +
						   HobString(Integer::toString(pitch)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("BitMap") );
#else
	Debug::removeTraceCategory("BitMap");
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BitMap::adjustMargins( bool top, bool bottom )
{
	const size_t pitch( 2 * getBitMapPitch() );
	const size_t height( getBitMapBoundsHeight() );

	// check for bad bitmap
	if (( !base ) || ( pitch <= 0 ))
		return;

	// check for valid height and top of zero
	if (( !height ) || ( 0 != getBitMapBoundsTop() ))
		return;

	Debug::trace( "BitMap.margins", "Adjusting bitmap margins!" );
	Debug::trace( "BitMap.margins", "    bitmap size " + util_string::inttostring( getBitMapBaseSize() ) );
	Debug::trace( "BitMap.margins", "   bitmap pitch " + util_string::inttostring( pitch ) );
	Debug::trace( "BitMap.margins", "   bitmap width " + util_string::inttostring( getBitMapBoundsWidth() ) );
	Debug::trace( "BitMap.margins", "  bitmap height " + util_string::inttostring( height ) );

	// ------------------------------------------------------------------------
	// setup mask
	// ------------------------------------------------------------------------

	std::vector<UINT8> mask;
	mask.resize( pitch, 0 );

	for ( size_t x( getBitMapBoundsWidth() ); x--; )
	{
		const size_t offset( x / 8 );
		const size_t bit( 7 - (x % 8) );

		mask[offset] |= (0x01 << bit);
	}

	HobString maskString;

	for ( size_t x( 0 ); x < pitch; ++x )
		maskString.append( Hex::toString( mask[x] ) );

	Debug::trace( "BitMap.margins", "  mask " + maskString );

	// ------------------------------------------------------------------------
	// determine what lines are used
	// ------------------------------------------------------------------------

	std::vector<bool> used;
	used.resize( height, false );

	for ( size_t y( 0 ); y < height; ++y )
	{
		HobString dataString;

		for ( size_t x( 0 ); x < pitch; ++x )
		{
			const UINT8 data( reinterpret_cast<UINT8*>( base )[pitch*y + x] );

			dataString.append( Hex::toString( data ) );

			if ( data & mask[x] )
				used[y] = true;
		}

		Debug::trace( "BitMap.margins", "  line " + util_string::inttostring( y ) + " data " + dataString + " used " + util_string::inttostring( used[y] ) );
	}

	// ------------------------------------------------------------------------
	// shrink bounds
	// ------------------------------------------------------------------------

	if ( bottom )
	{
		for ( size_t y( height ); y--; )
		{
			if ( used[y] )
				break;
			else if ( y <= (size_t) getBitMapBoundsTop() )
				break;

			bounds.addToRectangleBottom( -1 );
		}
	}

	if ( top )
	{
		size_t y( 0 );

		while ( y < (size_t) getBitMapBoundsBottom() )
		{
			if ( used[y++] )
				break;

			bounds.addToRectangleTop( 1 );
		}
	}

	Debug::trace( "BitMap.margins", "New bitmap margins:" );
	Debug::trace( "BitMap.margins", "     bitmap top " + util_string::inttostring( getBitMapBoundsTop() ) );
	Debug::trace( "BitMap.margins", "  bitmap bottom " + util_string::inttostring( getBitMapBoundsBottom() ) );
	Debug::trace( "BitMap.margins", "  bitmap height " + util_string::inttostring( getBitMapBoundsHeight() ) );

	// move bitmap up
	const size_t newHeight( getBitMapBoundsHeight() );

	memmove(
		&reinterpret_cast<UINT8*>( base )[0],
		&reinterpret_cast<UINT8*>( base )[pitch*getBitMapBoundsTop()],
		pitch * newHeight );

	bounds.addToRectangleBottom( -getBitMapBoundsTop() );
	bounds.addToRectangleTop( -getBitMapBoundsTop() );
}
