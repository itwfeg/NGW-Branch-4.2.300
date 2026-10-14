#ifndef REMOVESESSIONSTARTERMESSAGE_H
#define REMOVESESSIONSTARTERMESSAGE_H



#include "EvolMessage.h"


#include "SessionManager.h"



class RemoveSessionStarterMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		RemoveSessionStarterMessage(SessionManager::SessionStarter* starter);
		RemoveSessionStarterMessage(const char * msgData);
		~RemoveSessionStarterMessage();


		/* --- Getters and setters --- */
		SessionManager::SessionStarter* getSessionStarter() const { return starter_; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		SessionManager::SessionStarter* starter_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* REMOVESESSIONSTARTERMESSAGE_H */
