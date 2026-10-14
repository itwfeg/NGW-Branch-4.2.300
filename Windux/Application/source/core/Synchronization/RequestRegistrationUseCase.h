#ifndef REQUESTREGISTRATIONUSECASE_H
#define REQUESTREGISTRATIONUSECASE_H



#include "ISynchronizationUseCase.h"


#include "CommunicationSynchronizer.h"



namespace synchronization
{


	/* "Use Case 6.12: Requesting registration". */
	class RequestRegistrationUseCase : public ISynchronizationUseCase
	{
		public:
			/* --- Constructors and destructor --- */
			RequestRegistrationUseCase(CommunicationSynchronizer* synchronizer);
			~RequestRegistrationUseCase();


			/* --- Instance function members --- */
			bool doInitialize() { return true; }
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
			/* --- Instance Data members --- */
			CommunicationSynchronizer* synchronizer_;

			
			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static const std::string _dataTypeName_;
#endif
	};


}



#endif /* REQUESTREGISTRATIONUSECASE_H */
