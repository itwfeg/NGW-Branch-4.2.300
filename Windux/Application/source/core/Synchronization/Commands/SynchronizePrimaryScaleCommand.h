#ifndef SYNCHRONIZEPRIMARYSCALECOMMAND_H
#define SYNCHRONIZEPRIMARYSCALECOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class SynchronizePrimaryScaleCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		SynchronizePrimaryScaleCommand(int scaleId, ProcessedMessage* message = 0);
		SynchronizePrimaryScaleCommand(const char * buffer);
		virtual ~SynchronizePrimaryScaleCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SynchronizePrimaryScaleCommand(const SynchronizePrimaryScaleCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		SynchronizePrimaryScaleCommand& operator = (const SynchronizePrimaryScaleCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* SYNCHRONIZEPRIMARYSCALECOMMAND_H */
