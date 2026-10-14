#ifndef REQUESTDEREGISTRATIONCOMMAND_H
#define REQUESTDEREGISTRATIONCOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class RequestDeregistrationCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		RequestDeregistrationCommand(int targetScaleId, ProcessedMessage* message = 0);
		RequestDeregistrationCommand(const char * buffer);
		virtual ~RequestDeregistrationCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		RequestDeregistrationCommand(const RequestDeregistrationCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		RequestDeregistrationCommand& operator = (const RequestDeregistrationCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* REQUESTDEREGISTRATIONCOMMAND_H */
