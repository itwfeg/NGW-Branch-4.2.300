#ifndef ABORTBATCHCOMMAND_H
#define ABORTBATCHCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class AbortBatchCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		AbortBatchCommand(int batchId, ProcessedMessage* message = 0);
		AbortBatchCommand(const char * buffer);
		virtual ~AbortBatchCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		AbortBatchCommand(const AbortBatchCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		AbortBatchCommand& operator = (const AbortBatchCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* ABORTBATCHCOMMAND_H */
