#ifndef REQUESTREGISTRATIONPROCESSEDMESSAGE_H
#define REQUESTREGISTRATIONPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class RequestRegistrationProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		RequestRegistrationProcessedMessage(int status = 0);
		RequestRegistrationProcessedMessage(const char* buffer);
		RequestRegistrationProcessedMessage(const RequestRegistrationProcessedMessage& sm);
		virtual ~RequestRegistrationProcessedMessage();


		/* --- Getters and setters --- */


		/* --- Function members --- */
		/* Operators*/
		RequestRegistrationProcessedMessage& operator = (const RequestRegistrationProcessedMessage& rhs);


	private:
		/* --- Instance Data members --- */


		/* --- Constructors and destructor --- */


		/* --- Instance Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* REQUESTREGISTRATIONPROCESSEDMESSAGE_H */
