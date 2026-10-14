#ifndef OPERATORTOTALMANAGER_H
#define OPERATORTOTALMANAGER_H



#include <string>
#include <map>


#include "DataManager.h"
#include "OperatorTotal.h"


#include "CSmartPtr.h"


class SessionMessage;
class DBSession;


namespace hfc
{


	class HFCManagerContainer;
	class ReadFilteredOperatorTotalsMessage;


	class OperatorTotalManager : public DataManager
	{
		public:
			/* --- Types and subtypes --- */
			typedef std::map< std::string, Util::CSmartPtr<OperatorTotal> > operatorTotalsContainer_type;


			/* --- Constructors and destructor --- */
			OperatorTotalManager(HFCManagerContainer * container);
			~OperatorTotalManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() const { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }


			/* --- Instance function members --- */
			void handleMessage(SessionMessage* msg);
			void handleReadFilteredOperatorTotals(ReadFilteredOperatorTotalsMessage* message);

			int initializeAllOperatorTotalsQuery();
 			Util::CSmartPtr<OperatorTotal> getFirstOperatorTotal();
			Util::CSmartPtr<OperatorTotal> getNextOperatorTotal();


		private:
			/* --- Instance data members --- */
			DBSession * mySession_;
			HFCManagerContainer * mgrContainer_;
			operatorTotalsContainer_type cachedOperatorTotals_;
			operatorTotalsContainer_type::iterator iterator_;


			/* --- Instance function members --- */
			void clearCachedOperatorTotals();
			Util::CSmartPtr<OperatorTotal> getCachedOperatorTotal(const std::string& id);


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //OPERATORTOTALMANAGER_H
