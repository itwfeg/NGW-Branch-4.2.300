#ifndef HFCEVOLWRAPPERKNIFETIMEHIST
#define HFCEVOLWRAPPERKNIFETIMEHIST

#include "evolution.h"
//#include "EvolWrapperKnifeTime.h"
#include "EvolWrapperKnifeHistory.h"
#include "HfcObject.h"
#include <map>


class HfcEvolWrapperKnifeTimeHist : public HfcObject
{
	public:
	
		HfcEvolWrapperKnifeTimeHist();
		~HfcEvolWrapperKnifeTimeHist();

		//void addKnifeTime(const EvolWrapperKnifeTime& knifeTimeInstall);	
		void addKnifeTime(EvolWrapperKnifeHistory *pKnifeTime);

		virtual const char *serialize(int format = BrowserFormat)const;

	private:
			

		std::map<EvolWrapperKnifeHistory::KnifeHistoryType, EvolWrapperKnifeHistory *> knifeHistoryMap;
	
};
#endif