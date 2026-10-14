#ifndef KNIFETIMEHISTORY
#define KNIFETIMEHISTORY

#include "HfcObject.h"
#include "HobDateTime.h"
#include "evolution.h"

class DBAccess;
namespace edb
{
	class DBKnifeTimeHistory;
}

class KnifeTimeHistory : public HfcObject
{
	public:

		typedef enum 
		{
			KnifeTimeNone,
			KnifeUpTimeInstall,
			KnifeUpTimeMin,
			KnifeUpTimeMax,
			KnifeDownTimeInstall,
			KnifeDownTimeMin,
			KnifeDownTimeMax
		} KnifeHistoryType;

		KnifeTimeHistory(DBAccess *access = 0);
		KnifeTimeHistory(edb::DBKnifeTimeHistory *dbknifeTimeHistoryRec, DBAccess *access = 0);

		~KnifeTimeHistory();

		KnifeHistoryType getKnifeHistoryType() const { return knifeHistoryType_; }
		void setKnifeHistoryType(KnifeHistoryType knifeHistoryType) { knifeHistoryType_ = knifeHistoryType; }

		UINT32 getKnifeTime() const { return knifeTime_; }
		void setKnifeTime(UINT32 time) { knifeTime_ = time; }
		void setKnifeTime(std::string timeStr);

		HobDateTime getKnifeDateTime() const;
		void setKnifeDateTime(HobDateTime date);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		static KnifeTimeHistory *findKnifeTimeHistory(KnifeTimeHistory::KnifeHistoryType type, DBAccess *access = 0);
		void saveRecord();
		void show();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:
		DBAccess *dbAccess;
		edb::DBKnifeTimeHistory *myDbRecord;
		KnifeHistoryType knifeHistoryType_;
		UINT32 knifeTime_;
		HobDateTime	knifeDateTime_;
		HobErrorCode errorCode;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif		//	KNIFETIMEHISTORY
