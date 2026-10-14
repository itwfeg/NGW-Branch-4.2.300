#ifndef OPERATORCHANGEMANAGER_H
#define OPERATORCHANGEMANAGER_H



#include "DataManager.h"
#include "OperatorChangeSummary.h"
#include "OperatorChange.h"


#include "CSmartPtr.h"


class WriteSingleField;
class SessionMessage;
class DBSession;


namespace hfc
{


	class HFCManagerContainer;
	class Transaction;
	class TransactionManager;
	class ReadOperatorChangeSummaryMessage;
	class ReadOperatorChangeGroupMessage;
	class WriteOperatorChangeGroupMessage;


	class OperatorChangeManager : public DataManager
	{
		public:
			/* --- Types and subtypes --- */
			typedef std::map< std::string, Util::CSmartPtr<OperatorChangeSummary> > operatorSummaryContainer_type;


			/* --- Constructors and destructor --- */
			OperatorChangeManager(HFCManagerContainer * container);
			~OperatorChangeManager();


			/* --- Getter and setter members --- */
			DBSession * getMySession() const { return mySession_; }
			void setMySession(DBSession * session) { mySession_ = session; }


			/* --- Instance function members --- */
			void handleMessage(SessionMessage* msg);
			void handleReadOperatorChangeSummary(ReadOperatorChangeSummaryMessage* message);
			void handleReadOperatorChangeGroup(ReadOperatorChangeGroupMessage* message);

			int initializeAllOperatorChangesQuery();
 			Util::CSmartPtr<OperatorChange> getFirstOperatorChange();
			Util::CSmartPtr<OperatorChange> getNextOperatorChange();

//			Util::CSmartPtr<OperatorChange> createOperatorChange(WriteSingleField *productUpdate, HobString& newValue);
//			Util::CSmartPtr<OperatorChange> createOperatorChange(HobRecordNum productNum,
//					OperatorChange::OperatorChangeableField opChangeField, long origValue, long newValue);


		private:
			/* --- Instance data members --- */
			DBSession * mySession_;
			HFCManagerContainer * mgrContainer_;
			operatorSummaryContainer_type cachedOperatorSummaries_;
			operatorSummaryContainer_type::iterator summaryIterator_;

			HobRecordNum changeGroupTransType_;
			std::string changeGroupOperatorId_;
			std::string changeGroupOperatorName_;
			OperatorChange::OperatorChangeableField topTransChangeType_, bottomTransChangeType_;
			Util::CSmartPtr<Transaction> topChangeGroupTrans_, bottomChangeGroupTrans_;

			Util::CSmartPtr<Transaction> currentTransaction_;
			OperatorChange::OperatorChangeableField currentTransChangeType_;

			/* --- Instance function members --- */
			void clearCachedOperatorSummaries();
			Util::CSmartPtr<OperatorChangeSummary> getCachedOperatorSummary(const std::string& operatorId, const std::string& operatorName);
 			Util::CSmartPtr<OperatorChangeSummary> getFirstOperatorSummary();
			Util::CSmartPtr<OperatorChangeSummary> getNextOperatorSummary();

			void findFirstOperatorChange(TransactionManager* transManager, int viewWhatChanges = 0);	// defaults to viewing Operator Changes
			void findNextOperatorChange(TransactionManager* transManager, int viewWhatChanges = 0);		// defaults to viewing Operator Changes
			void findPreviousOperatorChange(TransactionManager* transManager, int viewWhatChanges = 0);	// ditto - default is 0
			Util::CSmartPtr<WriteOperatorChangeGroupMessage> getFirstOperatorChangeGroup(ReadOperatorChangeGroupMessage* inputMessage);
			Util::CSmartPtr<WriteOperatorChangeGroupMessage> getPreviousOperatorChangeGroup(int quantity, int viewWhatChanges = 0);
			Util::CSmartPtr<WriteOperatorChangeGroupMessage> getNextOperatorChangeGroup(int quantity, int viewWhatChanges = 0);


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif //OPERATORCHANGEMANAGER_H
