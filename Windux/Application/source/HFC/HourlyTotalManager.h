#ifndef HOURLYTOTALMANAGER_H
#define HOURLYTOTALMANAGER_H



#include "DataManager.h"
#include "HourlyTotal.h"


#include "CSmartPtr.h"


class SessionMessage;
class DBSession;


namespace hfc
{


	class HFCManagerContainer;
	class ReadFilteredHourlyTotalsMessage;


	class HourlyTotalManager : public DataManager
	{
		public:
			/* --- Constructors and destructor --- */
			HourlyTotalManager(HFCManagerContainer * container);
			~HourlyTotalManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() const { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }


			/* --- Instance function members --- */
			void handleMessage(SessionMessage* msg);
			void handleReadFilteredHourlyTotals(ReadFilteredHourlyTotalsMessage* message);

			int initializeAllHourlyTotalsQuery();
 			Util::CSmartPtr<HourlyTotal> getFirstHourlyTotal();
			Util::CSmartPtr<HourlyTotal> getNextHourlyTotal(bool onlyNotEmpty = true);


		private:
			/* --- Instance data members --- */
			DBSession * mySession_;
			HFCManagerContainer * mgrContainer_;
			int firstHour_, lastHour_, iterator_;
			std::vector< Util::CSmartPtr<HourlyTotal> > cachedHourlyTotals_;


			/* --- Instance function members --- */
			void clearCachedHourlyTotals();
			void updateCachedHourlyTotalsIterator();
			Util::CSmartPtr<HourlyTotal> getCachedHourlyTotal(short hour);


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //HOURLYTOTALMANAGER_H
