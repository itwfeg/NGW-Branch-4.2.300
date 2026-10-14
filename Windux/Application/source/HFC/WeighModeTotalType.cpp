/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeighModeTotalType.h"
#include "HobTokenizerCSV.h"
#include "TransactionType.h"
#include "Integer.h"
#include "Hex.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeighModeTotalType::dataType = 0;
std::string WeighModeTotalType::dataTypeName("WeighModeTotalType"); 
#endif

WeighModeTotalType::WeighModeTotalType()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


WeighModeTotalType::WeighModeTotalType(OpModeEvent runMode, hfc::TransactionType* runTransactionType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	runWeighMode = runMode;
	runModeTransactionType = runTransactionType;
	if (Debug::isTracing("TransactionType"))
	{
		HobString msgDebug("WeighModeTotalType::WeighModeTotalType(");
		msgDebug.append(Integer::toString(runWeighMode));
		msgDebug.append(",").append(util_string::ptrtostring(runModeTransactionType)).append(")");
		Debug::trace("TransactionType", msgDebug);
	}
}


WeighModeTotalType::WeighModeTotalType(const char *data, int len, int formatType)
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
			// format : WeighMode, TotalType

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case 1:
						setRunWeighMode((OpModeEvent) Integer::valueOf(token.c_str()));
						break;
					default:	//	The Total type will be sent in a different message.
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


WeighModeTotalType::~WeighModeTotalType()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void WeighModeTotalType::init()
{
	runWeighMode = OpModeIdle;
	runModeTransactionType = 0;
}


OpModeEvent WeighModeTotalType::getRunWeighMode()
{
	return runWeighMode;
}
void WeighModeTotalType::setRunWeighMode(OpModeEvent weighMode)
{
	runWeighMode = weighMode;
}


const char *WeighModeTotalType::serialize(int format) const
{
	serializedData = "";
	bool traceOn = Debug::isTracing("TotalType");
	HobString msgDebug("WeighModeTotalType::serialize():   ");
	switch (format)
	{
		case BrowserFormat:
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("runWeighMode = ").append(Integer::toString(runWeighMode));
				msgDebug2.append("runModeTransactionType = ").append(util_string::ptrtostring(runModeTransactionType));
				Debug::trace("TotalType", msgDebug2);
			}
			serializedData = "";
			serializedData.append(Integer::toString((int)runWeighMode));
			serializedData.append(BrowserFormatSeparator);
			if ( runModeTransactionType )
				serializedData.append(runModeTransactionType->serialize(CurrentRunFormat));
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("serializedData = ").append(serializedData);
		Debug::trace("TotalType", msgDebug2);
	}
	return serializedData.data();
}
