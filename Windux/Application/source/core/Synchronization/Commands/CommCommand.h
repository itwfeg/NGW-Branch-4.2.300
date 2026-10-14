#ifndef COMMCOMMAND_H
#define COMMCOMMAND_H



#include "Command.h"



class ProcessedMessage;



namespace synchronization
{


	class CommCommand : public Command
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		CommCommand(CommandType type, int targetScaleId, ProcessedMessage* message = 0);
		CommCommand(const char * buffer);
		virtual ~CommCommand();


		/* --- Getters and setters --- */
		bool isCommCommand() const { return true; }


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		CommCommand(const CommCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		CommCommand& operator = (const CommCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* COMMCOMMAND_H */
