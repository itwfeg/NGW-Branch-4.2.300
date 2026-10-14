#include "SMTPClientInterface.h"



#include "Configuration.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int SMTPClientInterface::_dataType_ = 0;
	std::string SMTPClientInterface::_dataTypeName_("SMTPClientInterface"); 
#endif

SMTPClientInterface::SMTPClientInterface() : client_(0) 
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
}

SMTPClientInterface::~SMTPClientInterface() 
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}

void SMTPClientInterface::setupSMTPServerParameters() const
{
	Configuration* pConfig = Configuration::getInstance();
	std::string debugmsg("");

	if ( pConfig )
	{
		// Retrieve the SMTP Server Host Name parameter from configuration
/*		parameter = config->getSMTPServerName();
		if ( parameter )*/
			client_->setSMTPServerName(std::string("NS3.HobartCorp.com"));//parameter);

		// Retrieve the SMTP Server IP Address parameter from configuration
		{
			HobString parameter;
			HobIpAddress ipAddr = pConfig->getSmtpIpAddrOrDnsName();
			ipAddr.getAsString(parameter);

			if ( parameter != "" )
			{
				debugmsg = "smtp server ip = " + parameter;

				Debug::trace("Email.SMTP", debugmsg);

				client_->setSMTPServerIP(parameter);
			}
		}
		// Retrieve the SMTP Server Port parameter from configuration
		int port = _SMTPWellKnownPort_;//config->getSMTPServerPort();
		if ( port )
			client_->setSMTPServerPort(port);
	}

	return;

}

void SMTPClientInterface::send(Email* email)
{
	// Lazy initialization of the actual client
	if ( !client_ )
	{
		client_ = new SMTPClient(*this);
		//lcg rev 0.1client_->start();

	}

	setupSMTPServerParameters();

	client_->addEmail(email);
}
