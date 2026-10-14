#ifndef NOTIFYLOCALBATCHESPRESENTUSECASE_H
#define NOTIFYLOCALBATCHESPRESENTUSECASE_H



#include "ISynchronizationUseCase.h"


#include "CommunicationSynchronizer.h"



namespace synchronization
{


	class NotifyLocalBatchesPresentUseCase : public ISynchronizationUseCase
	{
		public:
			/* --- Constructors and destructor --- */
			NotifyLocalBatchesPresentUseCase(CommunicationSynchronizer* synchronizer);
			~NotifyLocalBatchesPresentUseCase();


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
			CommunicationSynchronizer* synchronizer_;
			bool needToRegister_;
			bool operationFailed_;

			
			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static const std::string _dataTypeName_;
#endif
	};


}



#endif /* NOTIFYLOCALBATCHESPRESENTUSECASE_H */
