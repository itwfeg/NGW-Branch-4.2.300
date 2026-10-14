#ifndef HFCEVOLWRAPPERKNIFETIMEHISTORY
#define HFCEVOLWRAPPERKNIFETIMEHISTORY

#include "HfcObject.h"

class KnifeTimeHistory;
class DBAccess;

class HfcEvolWrapperKnifeTimeHistory : public HfcObject
{
	public:
	
		HfcEvolWrapperKnifeTimeHistory(DBAccess *access);
		~HfcEvolWrapperKnifeTimeHistory();

		virtual const char *serialize(int format = BrowserFormat) const;


	private:
		KnifeTimeHistory *pKnifeUpTimeInstall_;
		KnifeTimeHistory *pKnifeUpTimeMin_;
		KnifeTimeHistory *pKnifeUpTimeMax_;

		KnifeTimeHistory *pKnifeDownTimeInstall_;
		KnifeTimeHistory *pKnifeDownTimeMin_;
		KnifeTimeHistory *pKnifeDownTimeMax_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif		//	HFCEVOLWRAPPERKNIFETIMEHISTORY
