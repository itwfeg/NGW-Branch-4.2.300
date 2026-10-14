#ifndef SYNCHRONIZATIONSTATUS_H
#define SYNCHRONIZATIONSTATUS_H

#include "EvolMessage.h"
#include "Messages/SynchronizationDevEvent.h"

class SynchronizationStatus : public EvolMessage
{
	public:
		SynchronizationStatus(synchronization::SynchronizationDevError status, Hobart::CEvolMsgReceiver * pHobThread = 0);
		SynchronizationStatus(const char *buffer);
		virtual ~SynchronizationStatus();
		 
		const char *serialize() const;

		void setStatus(synchronization::SynchronizationDevError stat) {status = stat;}
		synchronization::SynchronizationDevError getStatus() const {return status;}


	private:
		synchronization::SynchronizationDevError status;


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //SYNCHRONIZATIONSTATUS_H
