#ifndef DELETEBATCHCOMMAND_H
#define DELETEBATCHCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class DeleteBatchCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		DeleteBatchCommand(int batchId, ProcessedMessage* message = 0);
		DeleteBatchCommand(const char * buffer);
		virtual ~DeleteBatchCommand();


		/* --- Getters and setters --- */


		/* --- Instance function members --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		DeleteBatchCommand(const DeleteBatchCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		DeleteBatchCommand& operator = (const DeleteBatchCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* DELETEBATCHCOMMAND_H */
