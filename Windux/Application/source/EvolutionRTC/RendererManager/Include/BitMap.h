#ifndef HOBBITMAP_H
#define HOBBITMAP_H

#include "Rectangle.h"

#include "CCriticalSection.h"

#include <list>

class Point;



// for this class:
// if you allocate a bitmap field, use allocateBitMap()
// if you allocate a bitmap then you must free it also!!
// to free it use freeBitMap()

class BitMap
{
	public:    
	    BitMap();
	    ~BitMap();
	        
	    INT16 getBitMapPitch() const;
	    void setBitMapPitch(INT16 mapDelta);    

	    UINT16 *getBitMapBase();
	    UINT32 getBitMapBaseSize() {return baseSize;}

		UINT16 *allocateBitMap(UINT32 size, bool fill = true, UINT8 fillChar = 0x00);
		void copyBitMap(UINT16 *source, UINT32 numWords);
		void freeBitMap();

	    void setBitMapBase(UINT16 *bitMapBase, UINT32 numWords = -1);
			
	    Hobart::Rectangle getBitMapBounds() const;
	    void setBitMapBounds(INT16 recLeft, INT16 recRight, INT16 recTop, INT16 recBottom);

		INT16 getBitMapBoundsTop() const;
		INT16 getBitMapBoundsBottom() const;
		INT16 getBitMapBoundsRight() const;
		INT16 getBitMapBoundsLeft() const;

		INT16 getBitMapBoundsWidth() const {return getBitMapBoundsRight() - getBitMapBoundsLeft();}
		INT16 getBitMapBoundsHeight() const {return getBitMapBoundsBottom() - getBitMapBoundsTop();}

		bool findPointInBounds(Point *point);

		void show() const;

		void adjustMargins( bool top = true, bool bottom = true );

	private:    
	    UINT16 *base;
	    UINT32 baseSize;
	    INT16 pitch;
	    Hobart::Rectangle bounds;

		static std::list<UINT16 *> bitmapsToDelete;
		static Util::CSmartCriticalSection bitmapsToDeleteCS;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif //HOBBITMAP_H
