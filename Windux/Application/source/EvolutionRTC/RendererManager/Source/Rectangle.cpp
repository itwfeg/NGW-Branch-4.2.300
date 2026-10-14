#include "Rectangle.h"
//#include "vxWorks.h"	//I belive that the macros for min and max are the same as the 302 stdlib macro's
#include "Point.h"


#include "Debug.h"
#include "Integer.h"
#include "HobString.h"
#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

#endif
#if defined(LINUX_FIX)
    #include <algorithm>
#endif

namespace Hobart
{

#ifdef USE_ACCOUNTANT
int Rectangle::dataType = 0;
std::string Rectangle::dataTypeName("Rectangle"); 
#endif

Rectangle::Rectangle(const Rectangle& obj):	left(obj.left), top(obj.top), right(obj.right), bottom(obj.bottom)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

Rectangle::Rectangle(INT16 recTop, INT16 recBottom, INT16 recRight, INT16 recLeft)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

    top 	= recTop;
	bottom 	= recBottom;
	right 	= recRight;
	left 	= recLeft;
}

Rectangle::~Rectangle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void Rectangle::show() const
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("Rectangle") );
#else
	Debug::addTraceCategory("Rectangle");
#endif
	Debug::trace("Rectangle", "Rectangle: ");

	Debug::trace("Rectangle", HobString("left: ") +
							  HobString(Integer::toString(left)));
	Debug::trace("Rectangle", HobString("right: ") +
							  HobString(Integer::toString(right)));
	Debug::trace("Rectangle", HobString("top: ") +
							  HobString(Integer::toString(top)));
	Debug::trace("Rectangle", HobString("bottom: ") +
							  HobString(Integer::toString(bottom)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("Rectangle") );
#else
	Debug::removeTraceCategory("Rectangle");
#endif
}

INT16  Rectangle::getRectangleBottom() const
{
	return bottom;
}
void Rectangle::setRectangleBottom(INT16 recBottom)
{
	bottom = recBottom;
}
void Rectangle::addToRectangleBottom(INT16 adjust)
{
	bottom = bottom + adjust;
}
void Rectangle::subtractFromRectangleBottom(INT16 adjust)
{
	bottom = bottom - adjust;
}


INT16 Rectangle::getRectangleTop() const
{
	return top;
}

void Rectangle::setRectangleTop(INT16 recTop)
{
	top = recTop;
}
void Rectangle::addToRectangleTop(INT16 adjust)
{
	top = top + adjust;
}
void Rectangle::subtractFromRectangleTop(INT16 adjust)
{
	top = top - adjust;
}


INT16 Rectangle::getRectangleLeft() const
{
	return left;
}
void Rectangle::setRectangleLeft(INT16 recLeft)
{
	left = recLeft;
}
void Rectangle::addToRectangleLeft(INT16 adjust)
{
	left = left + adjust;
}
void Rectangle::subtractFromRectangleLeft(INT16 adjust)
{
	left = left - adjust;
}


INT16 Rectangle::getRectangleRight() const
{
	return right;
}
void Rectangle::setRectangleRight(INT16 recRight)
{
	right = recRight;
}
void Rectangle::addToRectangleRight(INT16 adjust)
{
	right = right + adjust;
}
void Rectangle::subtractFromRectangleRight(INT16 adjust)
{
	right = right - adjust;
}

INT16 Rectangle::getRectangleWidth() const
{
	return right - left;
}
INT16 Rectangle::getRectangleHeight() const
{
	return bottom - top;
}

//will clip this rectangle to the clipping rectangle
// PRECONDITION: Rectangles MUST overlap!!!
void Rectangle::clipRect(Rectangle *clip)
{
#if defined(PAL_WIN32_RUN)
    top = max(top,clip->getRectangleTop());
    bottom = min(bottom,clip->getRectangleBottom());

    left = max(left,clip->getRectangleLeft());
    right = min(right,clip->getRectangleRight());

#else
    top = std::max(top,clip->getRectangleTop());
    bottom = std::min(bottom,clip->getRectangleBottom());

    left = std::max(left,clip->getRectangleLeft());
    right = std::min(right,clip->getRectangleRight());

#endif
}

void Rectangle::offsetRectangle(INT16 horzDelta, INT16 vertDelta)
{
	top		=  top + vertDelta;
	bottom 	= bottom + vertDelta;
	right	= right + horzDelta;
	left 	= left + horzDelta;		
}
void Rectangle::moveRectangleTo(INT16 recLeft, INT16 recTop)
{
	INT16 vertSize = 0;
	INT16 horizSize = 0;

	vertSize = bottom - top; 
	horizSize = right - left;

	top 	= 	recTop;
	recLeft = 	recLeft;
	bottom  = 	recTop + vertSize;
	right   = 	recLeft + horizSize;  

}
void Rectangle::setRectangleSize(INT16 horzSize, INT16 vertSize)
{
	bottom = top + vertSize;
	right = left + horzSize;

}
void Rectangle::stretchRectangle(INT16 vertDelta)
{
	top = top - vertDelta;
}

void Rectangle::shrinkRectangle(INT16 horzDelta, INT16 vertDelta)
{
	//proportionally shrink the rectangle by the delta amounts
	top 	=  top + vertDelta;
	bottom 	= bottom - vertDelta;
	left	= left + horzDelta;
	right	= right - horzDelta;
}
bool Rectangle::findPointInRectangle(const Point *point) const
{
	//This method will determine if a given point is in the current rectangle
	const INT16 horizPoint( point->getHorizontal() );
	const INT16 vertPoint( point->getVertical() );

	if ((horizPoint < left) ||
	    (horizPoint >= right) ||
	    (vertPoint < top) ||
	    (vertPoint >= bottom))
	{
		return false;
	}

    return true;
}

bool Rectangle::doesOverlap(const Rectangle& rect) const
{
	//This method will compare the coordinates of both rectangles and determine 
	//if the rectangles overlap
	if ((right < rect.left) ||
		(rect.right < left) ||
		(bottom < rect.top) ||
		(rect.bottom < top))
	{
		return false;
	}

	return true;
}

INT16 Rectangle::getWidth() const
{
	return getRectangleRight() - getRectangleLeft();
}

INT16 Rectangle::getHeight() const
{
	return getRectangleBottom() - getRectangleTop();
}

Rectangle& Rectangle::operator = ( const Rectangle &other )
{
    top = other.top;
	bottom	= other.bottom;
	right = other.right;
	left = other.left;

	return *this;
}

bool Rectangle::operator == ( const Rectangle& other ) const
{
    return (( top == other.top ) &&
		( bottom == other.bottom ) &&
		( right == other.right ) &&
		( left == other.left ));
}

bool Rectangle::operator != ( const Rectangle& other ) const
{
    return (( top != other.top ) ||
		( bottom != other.bottom ) ||
		( right != other.right ) ||
		( left != other.left ));
}

}//end of namespace Hobart
