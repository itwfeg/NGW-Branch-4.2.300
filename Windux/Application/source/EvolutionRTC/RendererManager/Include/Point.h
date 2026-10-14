#ifndef POINT_H
#define POINT_H

#include <string>
#include "PAL_platformheader.h"

class Point
{
public:
	Point(const Point& );
	Point(INT16 x = 0, INT16 y = 0);
	~Point();

	void show() const;

	INT16 getVertical() const;
	void setVertical(INT16 y);
	void addToVertical(INT16 delta);
	
	INT16 getHorizontal() const;
	void setHorizontal(INT16 x);
	void addToHorizontal(INT16 delta);

	void offSetCoordPoint(INT16 xOffSet, INT16 yOffSet);

private:

	INT16 h;	// hoizontal position of point
	INT16 v;	// vertical position of point
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //POINT_H
