#ifndef STARTSYNCMANAGERMESSAGE_H
#define STARTSYNCMANAGERMESSAGE_H



#include <string>


#include "EvolMessage.h"

#include "MSGEvent.h"



namespace synchronization
{


	class StartSyncManagerMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		StartSyncManagerMessage(bool online, Util::CMSGEvent& event);
		StartSyncManagerMessage(bool online, bool registration, const std::string & primaryScale, Util::CMSGEvent& event);
		StartSyncManagerMessage(const char* buffer);
		StartSyncManagerMessage(const StartSyncManagerMessage& sm);
		virtual ~StartSyncManagerMessage();


		/* --- Getters and setters --- */
		bool isOnLine() const { return onLine_; }
		void setOnLine(bool online) { onLine_ = online; }

		bool isRegistration() const { return registration_; }
		void setRegistration(bool registration) { registration_ = registration; }

		const std::string& getPrimaryScale() const { return primaryScale_; }
		void setPrimaryScale(const std::string& hostname) { primaryScale_ = hostname; }

		const Util::CMSGEvent& getEvent() const { return event_; }
//		void setEvent(const Util::CMSGEvent& event) { event_ = event; }


		/* --- Function members --- */
		/* Operators*/
		StartSyncManagerMessage& operator = (const StartSyncManagerMessage& rhs);
		bool signalEvent() { return event_.EventSet() != 0; }

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		bool onLine_;
		bool registration_;
		std::string primaryScale_;
		mutable Util::CMSGEvent event_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* STARTSYNCMANAGERMESSAGE_H */
