#ifndef HFCEVOLWRAPPERERRORHISTORY
#define HFCEVOLWRAPPERERRORHISTORY

#include "evolution.h"
#include "EvolWrapperErrorHistory.h"
#include "HfcObject.h"
#include <string.h>	
#include "DBWrapperErrorHistory.h"

class DBAccess;

	typedef enum HfcEvolWrapperErrorHistoryOffsets
	{
		fieldOffsetWrapperError = 1,
		fieldOffsetWrapperErrorDesc,
		fieldOffsetPackageLength,
		fieldOffsetPackageWidth,
		fieldOffsetPackageHeight,
		fieldOffsetFilmUsed,
		fieldOffsetUsedWrapBoxBestFit,
		fieldOffsetModifiedNetStretch,
		fieldOffsetTrayName,
		fieldOffsetDateTime,
		fieldOffsetEvolWrapperErrorHistoryEnd
	} HfcEvolWrapperErrorHistoryOffsets_t;

class HfcEvolWrapperErrorHistory : public EvolWrapperErrorHistory, public HfcObject
{
	public:
		HfcEvolWrapperErrorHistory(const char *data);
		HfcEvolWrapperErrorHistory(edb::DBWrapperErrorHistory *dbWrapperErrorHistory, DBAccess *access);

		~HfcEvolWrapperErrorHistory();

		void saveRecord();

		virtual const char *serialize(int format = BrowserFormat)const;

		void show();


	private:
		std::string dateTime;
		std::string errorDescription;

		DBAccess *dbAccess;

		edb::DBWrapperErrorHistory *myDbRecord;

		void init();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
