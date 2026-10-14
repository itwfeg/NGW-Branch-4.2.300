#ifndef DOWNLOADLOCALBATCHESUSECASE_H
#define DOWNLOADLOCALBATCHESUSECASE_H



#include "ISynchronizationUseCase.h"


#include "CommunicationSynchronizer.h"


#include "Scale.h"
#include "Batch.h"



namespace synchronization
{


	class DownloadLocalBatchesUseCase : public ISynchronizationUseCase
	{
		public:
			/* --- Constructors and destructor --- */
			DownloadLocalBatchesUseCase(CommunicationSynchronizer* synchronizer);
			~DownloadLocalBatchesUseCase();


			/* --- Instance function members --- */
			bool doInitialize();
			int doSendCommand();
			bool doProcessCommand(Util::CSmartPtr<SessionMessage> message);
			void doProcessError(CommSessionEvent::CommError error);
			void doConclude();

			void showUseCaseStartingInfo();
			void showUseCaseSuccessful();
			void showUseCaseFailed(const std::string& status);


		private:
			/* --- Instance data members --- */
			CommunicationSynchronizer* synchronizer_;
			Util::CSmartPtr<hfc::Scale> targetScale_;        // short cut to comm synchronizer's targetScale_
			bool notifiedBatches_;
			HobDateTime notifiedBatchesDateTime_;
			int startingVersionNumber_;
			Util::CSmartPtr<hfc::Batch> currentBatch_, receivedBatch_;
			bool operationFailed_;

			
			/* --- Instance function members --- */
			void processBatchEndCommand();


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif /* DOWNLOADLOCALBATCHESUSECASE_H */
