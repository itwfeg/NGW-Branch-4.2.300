#ifndef COMPLETECOMMANDMESSAGE_H
#define COMPLETECOMMANDMESSAGE_H



#include "EvolMessage.h"
//#include "Commands/Command.h"

#include "CSmartPtr.h"



namespace synchronization
{


	class Command;


	class CompleteCommandMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		CompleteCommandMessage(Util::CSmartPtr<Command> command);
		CompleteCommandMessage(const char* buffer);
		CompleteCommandMessage(const CompleteCommandMessage& sm);
		virtual ~CompleteCommandMessage();


		/* --- Getters and setters --- */
		Util::CSmartPtr<Command> getCommand() const { return command_; }
		void setCommand(Util::CSmartPtr<Command> command) { command_ = command; }


		/* --- Function members --- */
		/* Operators*/
		CompleteCommandMessage& operator = (const CompleteCommandMessage& rhs);

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



#endif /* COMPLETECOMMANDMESSAGE_H */
