#ifndef NOTIFYLOCALBATCHESPRESENTCOMMAND_H
#define NOTIFYLOCALBATCHESPRESENTCOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class NotifyLocalBatchesPresentCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		NotifyLocalBatchesPresentCommand(int targetScaleId, ProcessedMessage* message = 0)
			: CommCommand(NotifyLocalBatchesPresentCommandType, targetScaleId, message) {}
		NotifyLocalBatchesPresentCommand(const char * buffer);
		virtual ~NotifyLocalBatchesPresentCommand() {}


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		NotifyLocalBatchesPresentCommand(const NotifyLocalBatchesPresentCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		NotifyLocalBatchesPresentCommand& operator = (const NotifyLocalBatchesPresentCommand& rhs);
	};


}



#endif /* NOTIFYLOCALBATCHESPRESENTCOMMAND_H */
