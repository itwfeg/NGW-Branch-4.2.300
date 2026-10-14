#ifndef EXECUTECOMMANDMESSAGE_H
#define EXECUTECOMMANDMESSAGE_H



#include "EvolMessage.h"


#include "CSmartPtr.h"



namespace synchronization
{


	class Command;


	class ExecuteCommandMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ExecuteCommandMessage(Util::CSmartPtr<Command> command);
		ExecuteCommandMessage(const char* buffer);
		ExecuteCommandMessage(const ExecuteCommandMessage& sm);
		virtual ~ExecuteCommandMessage();


		/* --- Getters and setters --- */
		Util::CSmartPtr<Command> getCommand() const { return command_; }
		void setCommand(Util::CSmartPtr<Command> command) { command_ = command; }


		/* --- Function members --- */
		/* Operators*/
		ExecuteCommandMessage& operator = (const ExecuteCommandMessage& rhs);

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



#endif /* EXECUTECOMMANDMESSAGE_H */
