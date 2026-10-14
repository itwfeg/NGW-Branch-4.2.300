#ifndef APPLYBATCHCOMMAND_H
#define APPLYBATCHCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class ApplyBatchCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		ApplyBatchCommand(int batchId, ProcessedMessage* message = 0);
		ApplyBatchCommand(const char * buffer);
		virtual ~ApplyBatchCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		ApplyBatchCommand(const ApplyBatchCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		ApplyBatchCommand& operator = (const ApplyBatchCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* APPLYBATCHCOMMAND_H */
