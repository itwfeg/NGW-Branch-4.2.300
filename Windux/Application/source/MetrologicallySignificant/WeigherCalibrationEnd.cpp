/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherCalibrationEnd.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherCalibrationEnd::dataType = 0;
std::string WeigherCalibrationEnd::dataTypeName("WeigherCalibrationEnd"); 
#endif

WeigherCalibrationEnd::WeigherCalibrationEnd(short initialState)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	endState = initialState;
}

WeigherCalibrationEnd::WeigherCalibrationEnd(const char *data, int len, int formatType)
	: endState(-1)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			// format :	endState

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetEndState:
						setEndState(Integer::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

WeigherCalibrationEnd::~WeigherCalibrationEnd()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


short WeigherCalibrationEnd::getEndState() const
{
	return endState;
}
void WeigherCalibrationEnd::setEndState(short endState)
{
	this->endState = endState;
}


const char *WeigherCalibrationEnd::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
			HfcObject::serializedData = Integer::toString(endState);
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

