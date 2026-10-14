#ifndef REQUESTREGISTRATIONCOMMAND_H
#define REQUESTREGISTRATIONCOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class RequestRegistrationCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		RequestRegistrationCommand(int targetScaleId, ProcessedMessage* message = 0);
		RequestRegistrationCommand(const char * buffer);
		virtual ~RequestRegistrationCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		RequestRegistrationCommand(const RequestRegistrationCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		RequestRegistrationCommand& operator = (const RequestRegistrationCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* REQUESTREGISTRATIONCOMMAND_H */
