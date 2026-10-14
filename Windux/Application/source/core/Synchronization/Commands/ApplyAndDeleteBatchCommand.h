#ifndef APPLYANDDELETEBATCHCOMMAND_H
#define APPLYANDDELETEBATCHCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class ApplyAndDeleteBatchCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		ApplyAndDeleteBatchCommand(int batchId, ProcessedMessage* message = 0);
		ApplyAndDeleteBatchCommand(const char * buffer);
		virtual ~ApplyAndDeleteBatchCommand();


		/* --- Getters and setters --- */


		/* --- Instance function members --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		ApplyAndDeleteBatchCommand(const ApplyAndDeleteBatchCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		ApplyAndDeleteBatchCommand& operator = (const ApplyAndDeleteBatchCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* APPLYANDDELETEBATCHCOMMAND_H */
