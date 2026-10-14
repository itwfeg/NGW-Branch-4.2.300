#ifndef CLASSTOTALMANAGER_H
#define CLASSTOTALMANAGER_H



#include "DataManager.h"
#include "ClassTotal.h"


#include "CSmartPtr.h"


class SessionMessage;
class DBSession;


namespace hfc
{


	class HFCManagerContainer;


	class ClassTotalManager : public DataManager
	{
		public:
			/* --- Types and subtypes --- */
			typedef std::map< HobRecordNum, Util::CSmartPtr<ClassTotal> > classTotalsByTypeContainer_type;
			typedef std::map< HobRecordNum, Util::CSmartPtr<classTotalsByTypeContainer_type> > classTotalsContainer_type;


			/* --- Constructors and destructor --- */
			ClassTotalManager(HFCManagerContainer *container);
			~ClassTotalManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() const { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }


			/* --- Instance function members --- */
			void handleMessage(SessionMessage* msg);

			int initializeAllClassTotalsQuery(const std::string& deptNum);
 			Util::CSmartPtr<ClassTotal> getFirstClassTotal();
			Util::CSmartPtr<ClassTotal> getNextClassTotal();


		private:
			/* --- Instance data members --- */
			DBSession * mySession_;
			HFCManagerContainer *mgrContainer_;
			classTotalsContainer_type cachedClassTotals_;
			classTotalsContainer_type::iterator classIterator_;
			classTotalsByTypeContainer_type::iterator totalIterator_;


			/* --- Instance function members --- */
			void clearCachedClassTotals();
			Util::CSmartPtr<ClassTotal> getCachedClassTotal(HobRecordNum deptNum, HobRecordNum classNumber, HobRecordNum transTypeNumber);


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //CLASSTOTALMANAGER_H
