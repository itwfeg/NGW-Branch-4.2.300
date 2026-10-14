#ifndef ISYNCHRONIZATIONUSECASE_H
#define ISYNCHRONIZATIONUSECASE_H



#include "SessionMessage.h"


#include "CommunicationSynchronizerError.h"


namespace hfc
{
	class HeaderRecord;
}


#include "CSmartPtr.h"



namespace synchronization
{


	class ISynchronizationUseCase
	{
		public:
			virtual ~ISynchronizationUseCase() {}


			/* This is the API used by the CommSynchronizer to show that the
			use case is starting. */
			virtual void showUseCaseStartingInfo() = 0;


			/* This is the API necessary to perform any initialization.
			Return value:
				true - the initialization was completed
				false - still initializing
			*/
			virtual bool doInitialize() = 0;


			/* This is the API necessary to send commands to the target scale.
			Return value:
				2 - sent the last command
				1 - sent a command
				0 - trying to send a command
				-1 - error sending command
			*/
			virtual int doSendCommand() = 0;


			/* This is the API necessary to process commands received from the target scale.
			Return value:
				true - the command was processed
				false - still processing the command
			*/
			virtual bool doProcessCommand(Util::CSmartPtr<SessionMessage> message) = 0;


			/* This is the API necessary to process errors generated during the communication.
			Return value:
				void
			*/
			virtual void doProcessError(CommSessionEvent::CommError error) = 0;


			/* This is the API necessary to perform any concluding action.
			Return value:
				void
			*/
			virtual void doConclude() = 0;


			/* This is the API used by the CommSynchronizer to show that the
			use case has completed successfully. */
			virtual void showUseCaseSuccessful() = 0;


			/* This is the API used by the CommSynchronizer to show that the
			use case has failed. */
			virtual void showUseCaseFailed(const std::string& status) = 0;
	};


}



#endif /* ISYNCHRONIZATIONUSECASE_H */
