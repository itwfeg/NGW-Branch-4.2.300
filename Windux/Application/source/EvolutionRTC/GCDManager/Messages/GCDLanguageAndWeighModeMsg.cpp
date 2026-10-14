#include "GCDDevLanguageAndWeighModeMsg.h"
#include "CMReaderWriterGCD.h"

std::map< GcdLanguage, std::string > GCDDevLanguageAndWeighModeMsg::s_gcdLanguageMap;
std::map< WeighMode, std::string > GCDDevLanguageAndWeighModeMsg::s_weighModeMap;

GCDDevLanguageAndWeighModeMsg::GCDDevLanguageAndWeighModeMsg(GcdLanguage gcdLanguage, WeighMode weighMode) :
	EvolMessage(GCDDev, GcdLanguageAndWeighMode)
{
	serializedData.resize(0);
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, getLanguageStr(gcdLanguage));
	CMReaderWriterGCD::concatToData(serializedData, getWeighModeStr(weighMode));
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

GCDDevLanguageAndWeighModeMsg::GCDDevLanguageAndWeighModeMsg(WeighMode weighMode) :
	EvolMessage(GCDDev, GcdLanguageAndWeighMode)
{
	serializedData.resize(0);
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, getWeighModeStr(weighMode), 2);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

GCDDevLanguageAndWeighModeMsg::GCDDevLanguageAndWeighModeMsg(GcdLanguage gcdLanguage) :
	EvolMessage(GCDDev, GcdLanguageAndWeighMode)
{
	serializedData.resize(0);
	serializedData.reserve(16);
	CMReaderWriterGCD::setDataHeader(serializedData, eventType);
	CMReaderWriterGCD::concatToData(serializedData, "1", 0);
	CMReaderWriterGCD::concatToData(serializedData, getLanguageStr(gcdLanguage));
	CMReaderWriterGCD::concatDelimetersToData(serializedData, 1);
	CMReaderWriterGCD::resetDataHeaderLength(serializedData);
}

UINT32 GCDDevLanguageAndWeighModeMsg::getSerializedLength() const
{
	return static_cast< UINT >(serializedData.size());
}

const char *GCDDevLanguageAndWeighModeMsg::serialize() const
{
	return serializedData.data();
}

const std::string& GCDDevLanguageAndWeighModeMsg::getLanguageStr(GcdLanguage gcdLanguage)
{
	if (s_gcdLanguageMap.empty())
	{
		s_gcdLanguageMap.insert(std::pair< GcdLanguage, std::string >(GcdLanguage_English, "En"));
		s_gcdLanguageMap.insert(std::pair< GcdLanguage, std::string >(GcdLanguage_French, "Fr"));
		s_gcdLanguageMap.insert(std::pair< GcdLanguage, std::string >(GcdLanguage_Spanish, "Sp"));
	}
	return s_gcdLanguageMap[gcdLanguage];
}

const std::string& GCDDevLanguageAndWeighModeMsg::getWeighModeStr(WeighMode weighMode)
{
	// GCDR-19. Was checking for s_gcdLanguageMap to be empty. Probably a cut&paste typo.
	if (s_weighModeMap.empty())
	{
		s_weighModeMap.insert(std::pair< WeighMode, std::string >(WeighMode_Avoir, "Lb"));
		s_weighModeMap.insert(std::pair< WeighMode, std::string >(WeighMode_Metric, "Kg"));
	}
	return s_weighModeMap[weighMode];
}