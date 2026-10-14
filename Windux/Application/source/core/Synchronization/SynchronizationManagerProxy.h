#ifndef SYNCHRONIZATIONMANAGERPROXY_H
#define SYNCHRONIZATIONMANAGERPROXY_H



#include "MsgReceiverProxy.h"


#include "Batch.h"



namespace synchronization
{



	class SynchronizationManagerProxy : public Hobart::MsgReceiverProxy
	{
		public:
			/* --- Constructors and destructor --- */
			SynchronizationManagerProxy(Hobart::CEvolMsgReceiver* targetMsgReceiver);
			~SynchronizationManagerProxy();


			/* --- Instance Function members --- */
			/* "Use Case 5.20: Processing user changes". */
			bool processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange> record);
			/* "Use Case 5.21: Processing HQ batch". */
			bool processHQBatch(int batchNumber);
			/* "Use Case 5.28: Processing user file". */
			bool processUserFile(int batchNumber);
			/* "Use Case 5.37: Processing synchronized batch". */
			bool processSynchronizedBatch(int batchNumber);
			/* "Use Case 5.40: Creating batch". */
			bool createBatch(int* batchNumber, hfc::Batch::BatchOrigin origin, hfc::Batch::BatchType type);
			/* "Use Case 5.41: Adding record to batch". */
			bool addRecordToBatch(int batchNumber, Util::CSmartPtr<hfc::SynchronizationChange> record);
			/* "Use Case 5.42: Aborting batch". */
			bool abortBatch(int batchNumber);
			/* "Use Case 5.43: Completing batch". */
			bool completeBatch(int batchNumber);

			void stopProcessing() { setStopProcessing(); waitUntilIdle(); }


		private:
			/* --- Instance data members --- */


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};



}



#endif /*  SYNCHRONIZATIONMANAGERPROXY_H */
