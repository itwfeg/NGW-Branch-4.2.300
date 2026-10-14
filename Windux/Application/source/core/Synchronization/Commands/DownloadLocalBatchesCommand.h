#ifndef DOWNLOADLOCALBATCHESCOMMAND_H
#define DOWNLOADLOCALBATCHESCOMMAND_H



#include "CommCommand.h"



class ProcessedMessage;



namespace synchronization
{


	class DownloadLocalBatchesCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		DownloadLocalBatchesCommand(int scaleId, ProcessedMessage* message = 0);
		DownloadLocalBatchesCommand(const char * buffer);
		~DownloadLocalBatchesCommand();


		/* --- Getters and setters --- */


		/* --- Instance function members --- */


	private:
		/* --- Data members --- */


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		DownloadLocalBatchesCommand(const DownloadLocalBatchesCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		DownloadLocalBatchesCommand& operator = (const DownloadLocalBatchesCommand& rhs);


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* DOWNLOADLOCALBATCHESCOMMAND_H */
