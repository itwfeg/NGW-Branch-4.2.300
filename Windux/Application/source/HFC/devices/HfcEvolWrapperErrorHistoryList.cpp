#include "HfcEvolWrapperErrorHistoryList.h"
#include "EvolWrapperErrorTranslator.h"
#include "Configuration.h"
#include "Integer.h"
#include "Bool.h"

HfcEvolWrapperErrorHistoryList::HfcEvolWrapperErrorHistoryList()
					: currentSerializeOffset(0), totalErrors(0)
{

}

HfcEvolWrapperErrorHistoryList::~HfcEvolWrapperErrorHistoryList()
{

}
void HfcEvolWrapperErrorHistoryList::addToList(EvolWrapperErrorHistory *pEvolWrapperErrorHistory)
{
	unsigned int errorCode = (pEvolWrapperErrorHistory->getWrapperError() + 501);	//apply our offset
	errorCodeList_.push_back(Integer::toString(errorCode));
	
	HobString errorDesc = EvolWrapperErrorTranslator::getInstance()->translateEvolWrapperError(pEvolWrapperErrorHistory->getWrapperError());		
	errorDesc.quote();							//	Just in case there are commas in the text.
	errorDescList_.push_back(errorDesc);
	
	HobString lengthStr = "";
	Configuration::getInstance()->formatDecimalValue(pEvolWrapperErrorHistory->getPackageLength(), lengthStr, 1);
	packageLengthList_.push_back(lengthStr);
	

	HobString widthStr = "";
	Configuration::getInstance()->formatDecimalValue(pEvolWrapperErrorHistory->getPackageWidth(), widthStr, 1);
	packageWidthList_.push_back(widthStr);

	HobString heightStr = "";
	Configuration::getInstance()->formatDecimalValue(pEvolWrapperErrorHistory->getPackageHeight(), heightStr, 1);
	packageHeightList_.push_back(heightStr);
	
	
	filmUsedList_.push_back(Integer::toString(pEvolWrapperErrorHistory->getFilmUsed()));
	
	bestFitList_.push_back(Bool::toString(pEvolWrapperErrorHistory->getUsedBestFit()));
	
	netStretchList_.push_back(Bool::toString(pEvolWrapperErrorHistory->getStretchModified()));
	
	HobString trayName = pEvolWrapperErrorHistory->getTrayName();
	trayName.quote();							//	Just in case there are commas in the tray name.
	trayNameList_.push_back(trayName);

	HobString dateTime = pEvolWrapperErrorHistory->getDateTime();
	HobDateTime dt;
	dt.convertFromDBFormat(&dateTime);
	dateTime = dt.formatDate(FormatDate_MMDDYY);
	dateTime.append(",").append(dt.formatTime(FormatTime_HHMMSS, HourFormat_MILITARY));
	dateTime.quote();
	dateTimeList_.push_back(dateTime);

	++totalErrors;
}

void HfcEvolWrapperErrorHistoryList::gotoFirstPage()
{
	currentSerializeOffset = 0;
}
void HfcEvolWrapperErrorHistoryList::gotoNextPage()
{
	currentSerializeOffset += maxWrapperErrorsPerGroup_;
	
	if (currentSerializeOffset > totalErrors)
		currentSerializeOffset = 0;

}
void HfcEvolWrapperErrorHistoryList::gotoPreviousPage()
{
	if (currentSerializeOffset > 0)
	currentSerializeOffset -= maxWrapperErrorsPerGroup_;

}

const char *HfcEvolWrapperErrorHistoryList::serialize(int format) const
{
	HfcObject::serializedData = "";	

	switch (format)
	{
		case BrowserFormat:
		{

			HfcObject::serializedData.append(inttostring(totalErrors));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(inttostring(currentSerializeOffset));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(errorCodeList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(errorDescList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packageLengthList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packageWidthList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(packageHeightList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(filmUsedList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(bestFitList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(netStretchList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(trayNameList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(dateTimeList_.serialize(format, currentSerializeOffset, maxWrapperErrorsPerGroup_));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
/*
	void HfcEvolWrapperErrorHistoryList::createList()
	{
		EvolWrapperErrorHistory	*pEvolWrapperErrorHistory;
		int recordCount = getWrapperErrorRecordCount();
		
		if(recordCount)
		{	
			pEvolWrapperErrorHistory = getFirstEvolWrapperErrorHistory();
			addToList(pEvolWrapperErrorHistory);
			recordCount--;
		}

		while(recordCount)
		{
			pEvolWrapperErrorHistory = getNextEvolWrapperErrorHistory();
			addToList(pEvolWrapperErrorHistory);
			recordCount--;
		}

	}
*/
