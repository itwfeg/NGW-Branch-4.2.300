#ifndef PURGEBATCHESCOMMAND_H
#define PURGEBATCHESCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class PurgeBatchesCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		PurgeBatchesCommand(ProcessedMessage* message = 0);
		PurgeBatchesCommand(const char * buffer);
		virtual ~PurgeBatchesCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		PurgeBatchesCommand(const PurgeBatchesCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		PurgeBatchesCommand& operator = (const PurgeBatchesCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PURGEBATCHESCOMMAND_H */
