#ifndef COMMUNICATIONSYNCHRONIZERERROR_H
#define COMMUNICATIONSYNCHRONIZERERROR_H



#include <string>


#include "CommunicationSynchronizerEvent.h"
#include "CommSessionEvent.h"



namespace synchronization
{


	class CommunicationSynchronizerError : public CommunicationSynchronizerEvent
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructors --- */
			CommunicationSynchronizerError(CommSessionEvent::CommError error)
				: CommunicationSynchronizerEvent(ERROR_EVENT), errorType_(error) {}
			~CommunicationSynchronizerError() {}


			/* --- Getters and setters --- */
			CommSessionEvent::CommError getError() const { return errorType_; }


			/* --- Instance function members --- */
//			const std::string& getErrorText() const;


		private:
			/* --- Instance data members --- */
			CommSessionEvent::CommError errorType_;


			/* --- Class data members --- */
	};



}



#endif // COMMUNICATIONSYNCHRONIZERERROR_H
