#ifndef SYNCHRONIZESECONDARYSCALEUSECASE_H
#define SYNCHRONIZESECONDARYSCALEUSECASE_H



#include "ISynchronizationUseCase.h"


#include "CommunicationSynchronizer.h"


#include "Scale.h"
#include "Batch.h"



namespace synchronization
{


	class SynchronizeSecondaryScaleUseCase : public ISynchronizationUseCase
	{
		public:
			/* --- Constructors and destructor --- */
			SynchronizeSecondaryScaleUseCase(CommunicationSynchronizer* synchronizer, int scaleId);
			~SynchronizeSecondaryScaleUseCase();


			/* --- Instance function members --- */
			bool doInitialize();
			int doSendCommand();
        #if defined(LINUX_FIX)
			bool doProcessCommand(Util::CSmartPtr<SessionMessage> /* message */) { return true; }
        #else
			bool doProcessCommand(Util::CSmartPtr<SessionMessage> message) { return true; }
        #endif
			void doProcessError(CommSessionEvent::CommError error);
			void doConclude();

			void showUseCaseStartingInfo();
			void showUseCaseSuccessful();
			void showUseCaseFailed(const std::string& status);


		private:
			/* --- Instance data members --- */
			CommunicationSynchronizer* synchronizer_;
			int scaleId_;
			Util::CSmartPtr<hfc::Scale> targetScale_;
			int startingVersionNumber_;
			bool uploadingWholeDatabase_;
			Util::CSmartPtr<hfc::Batch> wholeDatabaseBatch_;
			hfc::HFCManagerContainer::syncListIterator_type wholeDBIterator_;
			DataManager* wholeDBDataManager_;
			Util::CSmartPtr<HfcObject> wholeDBRecord_;
			EvolDeviceType wholeDBDataManagerDevice_;
			UINT8 wholeDBDataManagerWriteEvent_;
			bool operationFailed_;
			bool newBatch_;
			Util::CSmartPtr<hfc::Batch> currentBatch_;
			Util::CSmartPtr<hfc::SynchronizationChange> currentBatchChange_;
			int recordsPerSyncIteration_;
			int delayPerSyncIteration_;

			
			/* --- Instance function members --- */
			void initializeWholeDatabaseUpload();
			Util::CSmartPtr<hfc::Batch> getWholeDatabaseBatch();
			bool sendDeleteAllCommand(DataManager* dataManager);
			int sendCommand();


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static const std::string _dataTypeName_;
#endif
	};


}



#endif /* SYNCHRONIZESECONDARYSCALEUSECASE_H */
