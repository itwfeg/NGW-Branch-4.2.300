#ifndef NOTIFYONLINEUSECASE_H
#define NOTIFYONLINEUSECASE_H



#include "ISynchronizationUseCase.h"


#include "CommunicationSynchronizer.h"



namespace synchronization
{


	class NotifyOnlineUseCase : public ISynchronizationUseCase
	{
		public:
			/* --- Constructors and destructor --- */
			NotifyOnlineUseCase(CommunicationSynchronizer* synchronizer);
			~NotifyOnlineUseCase();


			/* --- Instance function members --- */
			bool doInitialize();
			int doSendCommand();
        #if defined(LINUX_FIX)
            // Alamy: Due to unusded parameter error (***** WARNING: seems the function is not well implemented)
			bool doProcessCommand(Util::CSmartPtr<SessionMessage> UNUSED(message)) { return true; }
        #else
			bool doProcessCommand(Util::CSmartPtr<SessionMessage> message) { return true; }
        #endif
			void doProcessError(CommSessionEvent::CommError error);
			void doConclude();

			void showUseCaseStartingInfo();
			void showUseCaseSuccessful();
			void showUseCaseFailed(const std::string& status);


		private:
			/* --- Instance Data members --- */
			CommunicationSynchronizer* synchronizer_;
			bool needToRegister_;
			bool needToDeregister_;
			bool needToNotifyBatches_;
			bool operationFailed_;

			
			/* --- Instance function members --- */


			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static const std::string _dataTypeName_;
#endif
	};


}



#endif /* NOTIFYONLINEUSECASE_H */
