/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherCalibrationStep.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
#include "WeigherCalibration.h"
#include "Weigher.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherCalibrationStep::dataType = 0;
std::string WeigherCalibrationStep::dataTypeName("WeigherCalibrationStep"); 
#endif

WeigherCalibrationStep::WeigherCalibrationStep(short stepNum, long desiredCounts, long maxError)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	stepNum_ = stepNum;
	desiredCounts_ = desiredCounts;
	maxError_ = maxError;
}

WeigherCalibrationStep::WeigherCalibrationStep(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
	case InternalFormat:
		// Why am I receiving a Weigher Status from the RTC????  I shouldn't be!
		break;

	case BrowserFormat:
	{
		Debug::trace("WeigherCalibration", "creating step from browser format");

		HobString temp(data, len);
		HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
		HobString token;
		short loopCnt = 1;

		while (tok.next(&token) != -1)
		{
			setReceived(loopCnt, (token.length() == 0) ? false : true);

			switch (loopCnt)
			{
			case fieldOffsetStepNum:
				setStepNum(Integer::valueOf(token.c_str()));
				break;
			case fieldOffsetZeroedCounts:
				setZeroedCounts(Long::valueOf(token.c_str()));
				break;
			case fieldOffsetNonZeroedCounts:
				setNonZeroedCounts(Long::valueOf(token.c_str()));
				break;
			case fieldOffsetDelta:
				setDelta(Long::valueOf(token.c_str()));
				break;
			case fieldOffsetZeroedCountsDisplayed:
				if ( (zeroedCountsForDisplayValid_ = (token.length() ? true : false)) )
					setZeroedCountsForDisplay(Long::valueOf(token.c_str()));
				break;
			default:
				break;
			}

			loopCnt++;
		}

		Debug::trace("WeigherCalibration", "done creating step from browser format");
		break;
	}

	default:
		break;
	}
}

WeigherCalibrationStep::~WeigherCalibrationStep()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void WeigherCalibrationStep::init()
{
	stepNum_ = 0;

	zeroedCounts_ =
		zeroedCountsForDisplay_ =
		nonZeroedCounts_ =
		delta_ =
		desiredCounts_ =
		error_ =
		maxError_ = 0L;

	zeroedCountsForDisplayValid_ = false;
}

long WeigherCalibrationStep::calculateError(long gainFactor, long baseCounts, WeigherKind WgKind)
{
	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	if (WgKind == WgUsbAWS)
	{
		unsigned long long int temp;	

		temp = (unsigned long long int)((unsigned long long int)gainFactor * ((unsigned long long int)nonZeroedCounts_ - (unsigned long long int)baseCounts));
		temp /= WeigherCalibration::ScalingConstant;
		error_ = (long)((unsigned long long int)desiredCounts_ - temp);
	}
	else
	{
		error_ = desiredCounts_ - ((gainFactor * (nonZeroedCounts_ - baseCounts)) / 10000);
	}

	if (Debug::isTracing("WeigherCalibration"))
	{
		const HobString h1(Long::toString(gainFactor));
		const HobString h2(Long::toString(baseCounts));

		Debug::trace("WeigherCalibration",
			HobString("Calculated step error for gain factor:") + h1 +
			HobString("  baseCounts:") + h2 + HobString(toString()));
	}

	return error_;
}

const char *WeigherCalibrationStep::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
	case InternalFormat:
		HfcObject::serializedData = "Serialized WeigherCalibrationStep : internal format";
		break;

	case BrowserFormat:
		Debug::trace("WeigherCalibration", "serializing step for browser format");

		HfcObject::serializedData = Integer::toString(stepNum_);
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Long::toString(zeroedCounts_));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Long::toString(delta_));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		HfcObject::serializedData.append(Long::toString(nonZeroedCounts_));
		HfcObject::serializedData.append(BrowserFormatSeparator);
		if (zeroedCountsForDisplayValid_)
			HfcObject::serializedData.append(Long::toString(zeroedCountsForDisplay_));
		else
			HfcObject::serializedData.append(HobString());
		break;

	case SMFormat:
		HfcObject::serializedData = "Serialized WeigherCalibrationStep : sm format";
		break;

	default:
		break;
	}

	return HfcObject::serializedData.data();
}
