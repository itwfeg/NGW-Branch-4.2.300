#ifndef NOTIFYONLINECOMMAND_H
#define NOTIFYONLINECOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class NotifyOnlineCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		NotifyOnlineCommand(int targetScaleId, ProcessedMessage* message = 0)
			: CommCommand(NotifyOnlineCommandType, targetScaleId, message) {}
		NotifyOnlineCommand(const char * buffer);
		virtual ~NotifyOnlineCommand() {}


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		NotifyOnlineCommand(const NotifyOnlineCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		NotifyOnlineCommand& operator = (const NotifyOnlineCommand& rhs);
	};


}



#endif /* NOTIFYONLINECOMMAND_H */
