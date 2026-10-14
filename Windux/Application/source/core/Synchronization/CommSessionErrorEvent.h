#ifndef COMMSESSIONERROREVENT_H
#define COMMSESSIONERROREVENT_H



#include "CommSessionEvent.h"



class CommSessionErrorEvent : public CommSessionEvent
{
	public:
		// Types and subtypes


		// Constructors and destructors
		CommSessionErrorEvent(CommSessionEvent::CommError error)
			: CommSessionEvent(ERROR_EVENT), errorType_(error) {}
		~CommSessionErrorEvent() {}


		// Accessor methods
		CommSessionEvent::CommError getError() const { return errorType_; }



	private:
		CommSessionEvent::CommError errorType_;
};



#endif // COMMSESSIONERROREVENT_H
