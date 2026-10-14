/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "TouchScreenCalibrationStep.h"
#include "HobTokenizerCSV.h"

#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TouchScreenCalibrationStep::dataType = 0;
std::string TouchScreenCalibrationStep::dataTypeName("TouchScreenCalibrationStep"); 
#endif

TouchScreenCalibrationStep::TouchScreenCalibrationStep(short stepNum)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	step = stepNum;
}

TouchScreenCalibrationStep::TouchScreenCalibrationStep(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetTSCStepNum:
						setStep(Integer::valueOf(token));
						break;

					case fieldOffsetTSCXCoordinate:
						setX(Integer::valueOf(token));
						break;

					case fieldOffsetTSCYCoordinate:
						setY(Integer::valueOf(token));
						break;

					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

TouchScreenCalibrationStep::~TouchScreenCalibrationStep()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

//private
void TouchScreenCalibrationStep::init()
{
	step = 0;
	x = 0;
	y = 0;
}


const char *TouchScreenCalibrationStep::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			serializedData = Integer::toString(step);
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
