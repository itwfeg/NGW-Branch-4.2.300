#ifndef RETRYCOMMANDMESSAGE_H
#define RETRYCOMMANDMESSAGE_H



#include "EvolMessage.h"
//#include "Commands/Command.h"

#include "CSmartPtr.h"



namespace synchronization
{


	class Command;


	class RetryCommandMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		RetryCommandMessage(Util::CSmartPtr<Command> command);
		RetryCommandMessage(const char* buffer);
		RetryCommandMessage(const RetryCommandMessage& sm);
		virtual ~RetryCommandMessage();


		/* --- Getters and setters --- */
		Util::CSmartPtr<Command> getCommand() const { return command_; }
		void setCommand(Util::CSmartPtr<Command> command) { command_ = command; }


		/* --- Function members --- */
		/* Operators*/
		RetryCommandMessage& operator = (const RetryCommandMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		Util::CSmartPtr<Command> command_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* RETRYCOMMANDMESSAGE_H */
