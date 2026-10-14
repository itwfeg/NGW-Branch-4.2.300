#ifndef RECTANGLE_H
#define RECTANGLE_H



#include "PAL_platformheader.h"
#include <string>
typedef __int16 INT16;

class Point;


namespace Hobart
{
class Rectangle 
{
public: 
	Rectangle(const Rectangle&);   
	Rectangle(INT16 recTop = 0,
			  INT16 recBottom = 0,
			  INT16 recRight = 0,
			  INT16 recLeft = 0);
    ~Rectangle();    

	void show() const;

    INT16 getRectangleRight() const;
    void setRectangleRight(INT16 recRight);
	void addToRectangleRight(INT16 adjust);
	void subtractFromRectangleRight(INT16 adjust);

    INT16 getRectangleLeft() const;
    void setRectangleLeft(INT16 recLeft);
	void addToRectangleLeft(INT16 adjust);
	void subtractFromRectangleLeft(INT16 adjust);

    INT16 getRectangleTop() const;
    void setRectangleTop(INT16 recTop);
	void addToRectangleTop(INT16 adjust);
	void subtractFromRectangleTop(INT16 adjust);

    INT16 getRectangleBottom() const;
    void setRectangleBottom(INT16 recBottom);
	void addToRectangleBottom(INT16 adjust);
	void subtractFromRectangleBottom(INT16 adjust);

	INT16 getRectangleWidth() const;
	INT16 getRectangleHeight() const;

	void clipRect(Rectangle *clip);
	void offsetRectangle(INT16 horzDelta, INT16 vertDelta);
	void moveRectangleTo(INT16 recLeft, INT16 recTop);
	void setRectangleSize(INT16 horzSize, INT16 vertSize);
	void stretchRectangle(INT16 vertDelta);
	void shrinkRectangle(INT16 horizDelta, INT16 vertDelta);
	bool findPointInRectangle(const Point *point) const;
	bool doesOverlap(const Rectangle& rect) const;

	INT16 getWidth() const;
	INT16 getHeight() const;

	Rectangle& operator = ( const Rectangle& other );

	bool operator == ( const Rectangle& other ) const;
	bool operator != ( const Rectangle& other ) const;

private:    

    INT16 left;
    INT16 top;
    INT16 right;
    INT16 bottom;

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};
}//end of namespace Hobart
#endif //RECTANGLE_H
