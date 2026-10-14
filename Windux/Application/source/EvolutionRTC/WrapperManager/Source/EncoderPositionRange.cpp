#include <iostream>
using namespace std;

#include "EncoderPositionRange.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EncoderPositionRange::dataType = 0;
std::string EncoderPositionRange::dataTypeName("EncoderPositionRange"); 
#endif

EncoderPositionRange::EncoderPositionRange(UINT16 lb, UINT16 ub)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	lowerBound = lb;
	upperBound = ub;
}

EncoderPositionRange::~EncoderPositionRange()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void EncoderPositionRange::show()
{
	cout << "lower Bound: " << lowerBound << endl;
	cout << "upper Bound: " << upperBound << endl;
}

bool EncoderPositionRange::isEncoderInRange(UINT16 ec)
{
	bool rv = false;
	if (lowerBound < upperBound)
	{
		if ((lowerBound <= ec) && (ec <= upperBound))
		{
			rv = true;
		}
	}
	else if (upperBound < lowerBound)
	{
		if ((lowerBound <= ec) || (ec <= upperBound))
		{
			rv = true;
		}
	}
	else
	{
		if (ec == lowerBound)
		{
			rv = true;
		}
	}
	return rv;
}
