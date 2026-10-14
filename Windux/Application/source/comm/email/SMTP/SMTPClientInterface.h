#ifndef SMTPCLIENTINTERFACE_H
#define SMTPCLIENTINTERFACE_H



#include <string>



#include "../EmailComponent.h"
#include "../OutgoingMailerI.h"
#include "SMTPClient.h"



class Email;
class SMTPClient;



class SMTPClientInterface : public OutgoingMailerI
{
	public:
		// Constructors and destructor
		SMTPClientInterface();
		~SMTPClientInterface();


		// Accessor methods


		// Processing methods
		void setupSMTPServerParameters() const;
		void send(Email* email);



	private:
		SMTPClient* client_;


		static const int _SMTPWellKnownPort_ = 25;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif // SMTPCLIENTINTERFACE_H
