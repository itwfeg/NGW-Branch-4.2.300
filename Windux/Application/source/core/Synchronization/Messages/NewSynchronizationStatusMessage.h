#ifndef NEWSYNCHRONIZATIONSTATUSMESSAGE_H
#define NEWSYNCHRONIZATIONSTATUSMESSAGE_H



#include "EvolMessage.h"

#include "Messages/SynchronizationDevEvent.h"



namespace synchronization
{


	class NewSynchronizationStatusMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		NewSynchronizationStatusMessage(SynchronizationDevError status);
		NewSynchronizationStatusMessage(const char* buffer);
		NewSynchronizationStatusMessage(const NewSynchronizationStatusMessage& sm);
		virtual ~NewSynchronizationStatusMessage();


		/* --- Getters and setters --- */
		SynchronizationDevError getStatus() const { return status_; }
		void setStatus(SynchronizationDevError status) { status_ = status; }


		/* --- Function members --- */
		/* Operators*/
		NewSynchronizationStatusMessage& operator = (const NewSynchronizationStatusMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		SynchronizationDevError status_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* NEWSYNCHRONIZATIONSTATUSMESSAGE_H */
