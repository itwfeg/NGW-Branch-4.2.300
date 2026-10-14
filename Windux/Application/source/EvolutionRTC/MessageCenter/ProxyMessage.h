#ifndef PROXYMESSAGE_H
#define PROXYMESSAGE_H



#include <string>


#include "EvolMessage.h"

#include "MSGEvent.h"


class EvolMessageWrapper;



namespace Hobart
{


	class ProxyMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProxyMessage(EvolMessage* originalMessage, Util::CMSGEvent& event, EvolMessageWrapper* response);
		ProxyMessage(const char* buffer);
		ProxyMessage(const ProxyMessage& sm);
		~ProxyMessage();


		/* --- Getters and setters --- */
		const std::string& getOriginalMessage() const { return originalMessage_; }
//		void setOriginalMessage(const std::string& message) { originalMessage_ = message; }

		const Util::CMSGEvent& getEvent() const { return event_; }
//		void setEvent(const Util::CMSGEvent& event) { event_ = event; }

		EvolMessageWrapper* getResponse() const { return response_; }
//		void setResponse(EvolMessageWrapper* response) { response_ = response; }


		/* --- Function members --- */
		/* Operators*/
		ProxyMessage& operator = (const ProxyMessage& rhs);
		bool signalEvent() { return event_.EventSet() != 0; }

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		std::string originalMessage_;
		mutable Util::CMSGEvent event_;
		EvolMessageWrapper* response_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROXYMESSAGE_H */
