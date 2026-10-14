#include "HfcEvolWrapperKnifeTimeHist.h"
#include "WrapperEvent.h"
#include "HobString.h"
#include "Integer.h"
#include "Event.h"
#include "Debug.h"

HfcEvolWrapperKnifeTimeHist::HfcEvolWrapperKnifeTimeHist()
{
	knifeHistoryMap.empty();
}

HfcEvolWrapperKnifeTimeHist::~HfcEvolWrapperKnifeTimeHist()
{
}


void HfcEvolWrapperKnifeTimeHist::addKnifeTime(EvolWrapperKnifeHistory *pKnifeTime)
{
	knifeHistoryMap.insert(std::make_pair(pKnifeTime->getType(), pKnifeTime));
}

const char *HfcEvolWrapperKnifeTimeHist::serialize(int format)const
{
	std::map<EvolWrapperKnifeHistory::KnifeHistoryType, EvolWrapperKnifeHistory * >::const_iterator i = knifeHistoryMap.find(EvolWrapperKnifeHistory::KnifeTimeInstall);
	HfcObject::serializedData = "";
	if(format == BrowserFormat)
	{
		//are all elements in the map?
		if(knifeHistoryMap.size() == EvolWrapperKnifeHistory::KnifeTimeMax)
		{	
			if(i->first == EvolWrapperKnifeHistory::KnifeTimeInstall)
			{
				//serialize the install time
				
				HfcObject::serializedData.append(i->second->serialize());
				HfcObject::serializedData.append(BrowserFormatSeparator);
			}
			
			i = knifeHistoryMap.find(EvolWrapperKnifeHistory::KnifeTimeMin);

			if(i->first == EvolWrapperKnifeHistory::KnifeTimeMin)
			{

				HfcObject::serializedData.append(i->second->serialize());
				HfcObject::serializedData.append(BrowserFormatSeparator);

			}
			i = knifeHistoryMap.find(EvolWrapperKnifeHistory::KnifeTimeMax);

			if(i->first == EvolWrapperKnifeHistory::KnifeTimeMax)
			{
				HfcObject::serializedData.append(i->second->serialize());
			}
		}
	}
	return 	HfcObject::serializedData.c_str();
}