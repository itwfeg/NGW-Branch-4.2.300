#ifndef COMMANDWRAPPER_H
#define COMMANDWRAPPER_H



#include "Command.h"



namespace synchronization
{


	class CommandWrapper
	{
	public:
		/* --- Constructors and destructor --- */
		CommandWrapper(const Command* command);
		~CommandWrapper();


		/* --- Getters and setters --- */


		/* --- Instance function members --- */
		const char * serialize() const;
		UINT32 getSerializedLength() const { return (UINT32)serializedData.length(); }


		/* --- Class function members --- */
		static Command* createCommand(const char * buffer);


	private:
		/* --- Data members --- */
		const Command* command_;
		mutable std::string serializedData;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		CommandWrapper(const CommandWrapper& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		CommandWrapper& operator = (const CommandWrapper& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* COMMANDWRAPPER_H */
