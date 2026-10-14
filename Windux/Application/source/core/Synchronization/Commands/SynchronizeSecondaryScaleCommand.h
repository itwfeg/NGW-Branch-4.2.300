#ifndef SYNCHRONIZESECONDARYSCALECOMMAND_H
#define SYNCHRONIZESECONDARYSCALECOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class SynchronizeSecondaryScaleCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		SynchronizeSecondaryScaleCommand(int scaleId, ProcessedMessage* message = 0);
		SynchronizeSecondaryScaleCommand(const char * buffer);
		virtual ~SynchronizeSecondaryScaleCommand();


		/* --- Getters and setters --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SynchronizeSecondaryScaleCommand(const SynchronizeSecondaryScaleCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		SynchronizeSecondaryScaleCommand& operator = (const SynchronizeSecondaryScaleCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* SYNCHRONIZESECONDARYSCALECOMMAND_H */
