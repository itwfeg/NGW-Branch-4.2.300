#ifndef PRODUCTTOTALMANAGER_H
#define PRODUCTTOTALMANAGER_H



#include "DataManager.h"
#include "ProductTotal.h"
#include "ProductRunTotal.h"

#include "CSmartPtr.h"


class SessionMessage;
class DBSession;


namespace hfc
{


	class HFCManagerContainer;
	class ReadFilteredProductTotalsMessage;
	class Transaction;


	class ProductTotalManager : public DataManager
	{
		public:
			/* --- Types and subtypes --- */
			typedef std::map< HobRecordNum, Util::CSmartPtr<ProductTotal> > productTotalsByTypeContainer_type;
			typedef std::map< HobRecordNum, Util::CSmartPtr<productTotalsByTypeContainer_type> > productTotalsContainer_type;


			/* --- Constructors and destructor --- */
			ProductTotalManager(HFCManagerContainer *container);
			~ProductTotalManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() const { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }


			/* --- Instance function members --- */
			void handleMessage(SessionMessage* msg);

			int initializeAllProductTotalsQuery(const std::string& deptNum);
 			Util::CSmartPtr<ProductTotal> getFirstProductTotal();
			Util::CSmartPtr<ProductTotal> getNextProductTotal();

			void updateAndDisplayProductRunTotal(Transaction *trans, bool isVoid = false);
			Util::CSmartPtr<ProductRunTotal> getCurrentProductRunTotal() { return currentProductRunTotal_; }
			void clearAndDisplayProductRunTotal();

			bool customRunTotalsLabelExists() const;
	        void handleProductPrintCustomRunTotalsLabel();

		private:
			/* --- Instance data members --- */
			DBSession * mySession_;
			HFCManagerContainer * mgrContainer_;
			productTotalsContainer_type cachedProductTotals_;
			productTotalsContainer_type::iterator productIterator_;
			productTotalsByTypeContainer_type::iterator totalIterator_;


			/* --- Instance function members --- */
			void clearCachedProductTotals();
			Util::CSmartPtr<ProductTotal> getCachedProductTotal(HobRecordNum deptNum, HobRecordNum productNumber, HobRecordNum transTypeNumber);
			void handleReadFilteredProductTotals(ReadFilteredProductTotalsMessage* message);

			Util::CSmartPtr<ProductRunTotal> currentProductRunTotal_;		//	Used to display the packages, total price and total weight on the Run screens.
			Util::CSmartPtr<ProductRunTotal> createNewProductRunTotal(Transaction *prodTransaction);


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //PRODUCTTOTALMANAGER_H
