#ifndef COOLTOTALMANAGER_H
#define COOLTOTALMANAGER_H



#include "DataManager.h"
#include "CoolTotal.h"


#include "CSmartPtr.h"


class SessionMessage;
class DBSession;



namespace hfc
{


    class HFCManagerContainer;
    class Transaction;


    class COOLTotalManager : public DataManager
    {
        public:
            /* --- Types and subtypes --- */
            typedef std::map< std::string, Util::CSmartPtr<COOLTotal> > coolTotalsByCodeContainer_type;
            typedef std::map< HobRecordNum, Util::CSmartPtr<coolTotalsByCodeContainer_type> > pluCoolTotalsContainer_type;
            typedef std::map< HobRecordNum, Util::CSmartPtr<pluCoolTotalsContainer_type> > coolTotalsContainer_type;


            /* --- Constructors and destructor --- */
            COOLTotalManager(HFCManagerContainer *container);
            ~COOLTotalManager();


            /* --- Getter and setter members --- */
            DBSession * getMySession() const { return mySession_; }
            void setMySession(DBSession * session) { mySession_ = session; }


            /* --- Instance function members --- */
            void handleMessage(SessionMessage* msg);

            int initializeAllCOOLTotalsQuery();
            Util::CSmartPtr<COOLTotal> getFirstCOOLTotal();
            Util::CSmartPtr<COOLTotal> getNextCOOLTotal();


        private:
            /* --- Instance data members --- */
            DBSession * mySession_;
            hfc::HFCManagerContainer *mgrContainer_;
            coolTotalsContainer_type cachedCOOLTotals_;
            coolTotalsContainer_type::iterator pluCoolIterator_;
            Util::CSmartPtr<pluCoolTotalsContainer_type> pluCoolTotals_;
            pluCoolTotalsContainer_type::iterator totalByCodeIterator_;
            Util::CSmartPtr<coolTotalsByCodeContainer_type> totalsByCode_;
            coolTotalsByCodeContainer_type::iterator totalIterator_;


            /* --- Instance function members --- */
            void clearCachedCOOLTotals();
            Util::CSmartPtr<COOLTotal> getCachedCOOLTotal(HobRecordNum productNumber, HobRecordNum transactionTypeNumber, std::string trackingCode, std::string coolText);
            //Util::CSmartPtr<COOLTotal> COOLTotalManager::createCOOLTotal(Util::CSmartPtr<Transaction> trans);
            Util::CSmartPtr<COOLTotal> createCOOLTotal(Util::CSmartPtr<Transaction> trans);


            /* --- Class data members --- */
#ifdef USE_ACCOUNTANT
            static int dataType;
            static std::string dataTypeName;
#endif
    };


}



#endif //COOLTOTALMANAGER_H
