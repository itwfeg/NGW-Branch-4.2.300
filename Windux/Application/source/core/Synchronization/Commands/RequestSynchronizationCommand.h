#ifndef REQUESTSYNCHRONIZATIONCOMMAND_H
#define REQUESTSYNCHRONIZATIONCOMMAND_H



#include "CommCommand.h"



class EvolMessage;



namespace synchronization
{


	class RequestSynchronizationCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		RequestSynchronizationCommand(EvolMessage* message = 0);
		RequestSynchronizationCommand(const char * buffer);
		virtual ~RequestSynchronizationCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		RequestSynchronizationCommand(const RequestSynchronizationCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		RequestSynchronizationCommand& operator = (const RequestSynchronizationCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* REQUESTSYNCHRONIZATIONCOMMAND_H */
