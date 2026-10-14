#include"Point.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Point::dataType = 0;
std::string Point::dataTypeName("Point"); 
#endif

Point::Point(const Point& obj): h(obj.h), v(obj.v)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

Point::Point(INT16 x, INT16 y)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	h = x;
	v = y;
}

Point::~Point()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void Point::show() const
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("Point") );
#else
	Debug::addTraceCategory("Point");
#endif
	Debug::trace("Point", "Point: ");

	Debug::trace("Point", HobString("(x, y): (") + HobString(Integer::toString(h)) +
						  HobString(", ") + HobString(Integer::toString(v)) +
						  HobString(")"));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("Point") );
#else
	Debug::removeTraceCategory("Point");
#endif
}

void Point::setVertical(INT16 y)
{
	v = y;
}
void Point::addToVertical(INT16 delta)
{
	v = v + delta;
}
INT16 Point::getVertical() const
{
	return v;
}

void Point::setHorizontal(INT16 x)
{
	h = x;
}
void Point:: addToHorizontal(INT16 delta)
{
	h = h + delta;
}
INT16 Point::getHorizontal() const
{
	return h;
}

void Point::offSetCoordPoint(INT16 xOffSet, INT16 yOffSet)
{
	h  = h + xOffSet;
	v  = v + yOffSet;
} 
